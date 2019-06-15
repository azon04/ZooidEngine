#include "GBufferRenderPass.h"
#include "ResourceManagers/ShaderManager.h"
#include "Resources/Texture.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/DrawList.h"
#include "SceneRenderer/SceneRenderer.h"
#include "SceneRenderer/RenderGatherer.h"
#include "ZEGameContext.h"

namespace ZE
{

	GBufferRenderPass::GBufferRenderPass() : m_shaderChain(nullptr), m_frameBuffer(nullptr)
	{

	}

	void GBufferRenderPass::prepare(GameContext* _gameContext)
	{
		ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());

		// Load shader for this pass
		if (!m_shaderChain)
		{
			m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/GBufferVertexShader.vs", "ZooidEngine/Shaders/DeferredShading/GBufferPixelShader.frag", nullptr, nullptr);
			m_shaderSkinChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/GBufferVertexShaderSkin.vs", "ZooidEngine/Shaders/DeferredShading/GBufferPixelShader.frag", nullptr, nullptr);
		}

		// Create Frame Buffers
		if (!m_frameBuffer)
		{
			TextureCreateDesc textureCreateDesc;
			textureCreateDesc.Width = _gameContext->getRenderer()->GetWidth();
			textureCreateDesc.Height = _gameContext->getRenderer()->GetHeight();
			textureCreateDesc.MinFilter = NEAREST;
			textureCreateDesc.MagFilter = NEAREST;
			textureCreateDesc.TextureFormat = TEX_RGBA16F;
			textureCreateDesc.DataType = FLOAT;
			textureCreateDesc.bGenerateMipMap = false;

			// Position Texture Buffer
			Handle textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_positionTexture = textureHandle.getObject<IGPUTexture>();
				m_positionTexture->create(textureCreateDesc);
			}

			// Normal Texture Buffer
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_normalTexture = textureHandle.getObject<IGPUTexture>();
				m_normalTexture->create(textureCreateDesc);
			}

			// Spec Texture Buffer
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_specTexture = textureHandle.getObject<IGPUTexture>();
				m_specTexture->create(textureCreateDesc);
			}

			// Ambient Texture Buffer
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_ambientTexture = textureHandle.getObject<IGPUTexture>();
				m_ambientTexture->create(textureCreateDesc);
			}

			// Albedo Texture Buffer
			textureCreateDesc.DataType = UNSIGNED_BYTE;
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_albedoTexture = textureHandle.getObject<IGPUTexture>();
				m_albedoTexture->create(textureCreateDesc);
			}

			

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
				m_frameBuffer = fbHandle.getObject<IGPUFrameBuffer>();
				m_frameBuffer->bind();
				m_frameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_positionTexture, 0);
				m_frameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_normalTexture, 1);
				m_frameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_albedoTexture, 2);
				m_frameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_specTexture, 3);
				m_frameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_ambientTexture, 4);
				m_frameBuffer->addRenderBufferAttachment(DEPTH_ATTACHMENT, m_depthRenderBuffer);
				m_frameBuffer->setupAttachments();
				m_frameBuffer->unbind();
			}
		}
	}

	void GBufferRenderPass::release(GameContext* _gameContext)
	{
		if (m_frameBuffer) { m_frameBuffer->release(); m_frameBuffer = nullptr; }
		if (m_ambientTexture) { m_ambientTexture->release(); m_ambientTexture = nullptr; }
		if (m_positionTexture) { m_positionTexture->release(); m_positionTexture = nullptr; }
		if (m_normalTexture) { m_normalTexture->release(); m_normalTexture = nullptr; }
		if (m_albedoTexture) { m_albedoTexture->release(); m_albedoTexture = nullptr; }
		if (m_specTexture) { m_specTexture->release(); m_specTexture = nullptr; }
		if (m_depthRenderBuffer) { m_depthRenderBuffer->release(); m_depthRenderBuffer = nullptr; }
	}

	void GBufferRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);
		ZCHECK(m_frameBuffer);
		m_frameBuffer->bind();
	}

	void GBufferRenderPass::end(GameContext* _gameContext)
	{
		RenderPass::end(_gameContext);
		// Add to outputs
		addOutputTextureBuffer(m_positionTexture);
		addOutputTextureBuffer(m_normalTexture);
		addOutputTextureBuffer(m_albedoTexture);
		addOutputTextureBuffer(m_specTexture);
		addOutputTextureBuffer(m_ambientTexture);
		addOutputFrameBuffer(m_frameBuffer);

		ZCHECK(m_frameBuffer);
		m_frameBuffer->unbind();
	}

	bool GBufferRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool GBufferRenderPass::execute_GPU(GameContext* _gameContext)
	{
		DrawList* drawList = _gameContext->getDrawList();

		_gameContext->getRenderer()->ResetViewport();
		_gameContext->getRenderer()->ClearScreen();

		MeshSceneRenderer::Render(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount(), m_shaderChain);
		SkinMeshSceneRenderer::Render(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount(), m_shaderSkinChain);
		//TransculentSceneRenderer::Render(drawList->m_transculentRenderGatherer.getRenderInfos(), drawList->m_transculentRenderGatherer.getRenderCount(), m_shaderChain);

		return true;
	}

}