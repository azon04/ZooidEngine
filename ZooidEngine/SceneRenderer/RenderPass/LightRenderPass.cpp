#include "LightRenderPass.h"
#include "Renderer/IRenderer.h"
#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/BufferManager.h"
#include "Resources/Texture.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/DrawList.h"
#include "SceneRenderer/ShadowRenderer.h"
#include "SceneRenderer/SkyboxRenderer.h"
#include "ZEGameContext.h"

ZE::LightRenderPass::LightRenderPass()
	: m_shaderChain(nullptr), m_resultPassTexture(nullptr), m_resultFrameBuffer(nullptr)
{}

void ZE::LightRenderPass::prepare(GameContext* _gameContext)
{
	ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());

	// Load shader for this pass
	if (!m_shaderChain)
	{
		m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/DeferredLightShader.vs", "ZooidEngine/Shaders/DeferredShading/DeferredLightShader.frag", nullptr, nullptr);
	}

	// Create Frame Buffer
	if (!m_resultFrameBuffer)
	{
		TextureCreateDesc textureCreateDesc;
		textureCreateDesc.Width = _gameContext->getRenderer()->GetWidth();
		textureCreateDesc.Height = _gameContext->getRenderer()->GetHeight();
		textureCreateDesc.MinFilter = NEAREST;
		textureCreateDesc.MagFilter = NEAREST;
		textureCreateDesc.TextureFormat = TEX_RGB16F;
		textureCreateDesc.DataType = UNSIGNED_BYTE;
		textureCreateDesc.bGenerateMipMap = false;

		// Result Pass Texture Buffer
		Handle textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
		if (textureHandle.isValid())
		{
			m_resultPassTexture = textureHandle.getObject<IGPUTexture>();
			m_resultPassTexture->create(textureCreateDesc);
		}

		// Depth Render Buffer
		Handle depthRenderBuffer = _gameContext->getRenderZooid()->CreateRenderBuffer();
		if (depthRenderBuffer.isValid())
		{
			m_depthRenderBuffer = depthRenderBuffer.getObject<IGPURenderBuffer>();
			m_depthRenderBuffer->create(DEPTH_ONLY, textureCreateDesc.Width, textureCreateDesc.Height);
		}

		// Create Frame Buffer
		Handle fbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
		if (fbHandle.isValid())
		{
			m_resultFrameBuffer = fbHandle.getObject<IGPUFrameBuffer>();
			m_resultFrameBuffer->bind();
			m_resultFrameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_resultPassTexture);
			m_resultFrameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthRenderBuffer);
			m_resultFrameBuffer->setupAttachments();
			m_resultFrameBuffer->unbind();
		}
	}
}

void ZE::LightRenderPass::release(GameContext* _gameContext)
{
	if (m_resultFrameBuffer) { m_resultFrameBuffer->release(); m_resultFrameBuffer = nullptr; }
	if (m_resultPassTexture) { m_resultPassTexture->release(); m_resultPassTexture = nullptr; }
}

void ZE::LightRenderPass::begin(GameContext* _gameContext)
{
	RenderPass::begin(_gameContext);
	ZCHECK(m_resultFrameBuffer);
	m_resultFrameBuffer->bind();
}

void ZE::LightRenderPass::end(GameContext* _gameContext)
{
	RenderPass::end(_gameContext);
	ZCHECK(m_resultFrameBuffer);
	m_resultFrameBuffer->unbind();
	addOutputTextureBuffer(m_resultPassTexture);
	addOutputFrameBuffer(m_resultFrameBuffer);
}

bool ZE::LightRenderPass::execute_CPU(GameContext* _gameContext)
{
	return true;
}

bool ZE::LightRenderPass::execute_GPU(GameContext* _gameContext)
{
	DrawList* drawList = _gameContext->getDrawList();
	IGPUBufferArray* quadVBO = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);

	m_shaderChain->bind();

	_gameContext->getDrawList()->m_lightConstantBuffer->bind();
	m_shaderChain->bindConstantBuffer("light_data", _gameContext->getDrawList()->m_lightConstantBuffer);

	// Bind Textures
	ZCHECK(m_textureBufferInputs.length() == 5);
	IGPUTexture* positionTexture = m_textureBufferInputs[0];
	IGPUTexture* normalTexture = m_textureBufferInputs[1];
	IGPUTexture* albedoTexture = m_textureBufferInputs[2];
	IGPUTexture* specTexture = m_textureBufferInputs[3];
	IGPUTexture* ambientTexture = m_textureBufferInputs[4];

	m_shaderChain->setTexture("gPosition", positionTexture, 0);
	positionTexture->bind();

	m_shaderChain->setTexture("gNormal", normalTexture, 1);
	normalTexture->bind();

	m_shaderChain->setTexture("gAlbedo", albedoTexture, 2);
	albedoTexture->bind();

	m_shaderChain->setTexture("gSpec", specTexture, 3);
	specTexture->bind();

	m_shaderChain->setTexture("gAmbient", ambientTexture, 4);
	ambientTexture->bind();

	ShadowDepthRenderer::BindShadowTextures(_gameContext->getDrawList(), m_shaderChain, 5);

	_gameContext->getRenderer()->DrawBufferArray(TOPOLOGY_TRIANGLE, quadVBO, 6);

	m_shaderChain->unbind();

	// Copy Depth Texture from Input Frame Buffer
	IGPUFrameBuffer* inputFrameBuffer = m_frameBufferInputs[0];
	ZCHECK(inputFrameBuffer);

	Int32 width = _gameContext->getRenderer()->GetWidth();
	Int32 height = _gameContext->getRenderer()->GetHeight();
	_gameContext->getRenderer()->CopyFrameBuffer(inputFrameBuffer, m_resultFrameBuffer, 0, 0, width, height, 0, 0, width, height, ERenderBufferBit::DEPTH_BUFFER_BIT, ETextureFilter::NEAREST);

	SkyBoxRenderer::Render(drawList->m_skyboxRenderGatherer.getRenderInfos(), drawList->m_skyboxRenderGatherer.getRenderCount());

	return true;
}
