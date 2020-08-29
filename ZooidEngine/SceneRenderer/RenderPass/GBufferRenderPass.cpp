#include "GBufferRenderPass.h"
#include "ResourceManagers/ShaderManager.h"
#include "Resources/Texture.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUStates.h"
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
			m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/GBufferVertexShader.vs", "ZooidEngine/Shaders/DeferredShading/PBR/GBufferPixelShader.frag", nullptr, nullptr);
			m_shaderSkinChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/GBufferVertexShaderSkin.vs", "ZooidEngine/Shaders/DeferredShading/PBR/GBufferPixelShader.frag", nullptr, nullptr);
		}

		// Create Frame Buffers
		if (!m_frameBuffer)
		{
			TextureCreateDesc textureCreateDesc;
			textureCreateDesc.Width = (UInt32)_gameContext->getRenderer()->GetWidth();
			textureCreateDesc.Height = (UInt32)_gameContext->getRenderer()->GetHeight();
			textureCreateDesc.MinFilter = NEAREST;
			textureCreateDesc.MagFilter = NEAREST;
			textureCreateDesc.WrapU = CLAMP_TO_EDGE;
			textureCreateDesc.WrapV = CLAMP_TO_EDGE;
			textureCreateDesc.DataType = FLOAT;
			textureCreateDesc.bGenerateMipMap = false;

			// Position Texture Buffer
			textureCreateDesc.TextureFormat = TEX_RGB16F;
			Handle textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_positionTexture = textureHandle.getObject<IGPUTexture>();
				m_positionTexture->create(textureCreateDesc);
				m_positionTexture->setDebugName("PositionBuffer");
			}

			// Normal Texture Buffer
			textureCreateDesc.TextureFormat = TEX_RGB16F;
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_normalTexture = textureHandle.getObject<IGPUTexture>();
				m_normalTexture->create(textureCreateDesc);
				m_normalTexture->setDebugName("NormalBuffer");
			}

			textureCreateDesc.TextureFormat = TEX_RGBA16F;
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_tMRF = textureHandle.getObject<IGPUTexture>();
				m_tMRF->create(textureCreateDesc);
				m_tMRF->setDebugName("MetalReflectReougnessBuffer");
			}

			// Ambient Texture Buffer
			textureCreateDesc.TextureFormat = TEX_RGB;
			textureCreateDesc.DataType = UNSIGNED_BYTE;
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_ambientTexture = textureHandle.getObject<IGPUTexture>();
				m_ambientTexture->create(textureCreateDesc);
				m_ambientTexture->setDebugName("AmbientBuffer");
			}

			// Albedo Texture Buffer
			textureCreateDesc.TextureFormat = TEX_RGB;
			textureCreateDesc.DataType = UNSIGNED_BYTE;
			textureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (textureHandle.isValid())
			{
				m_albedoTexture = textureHandle.getObject<IGPUTexture>();
				m_albedoTexture->create(textureCreateDesc);
				m_albedoTexture->setDebugName("AlbedoBuffer");
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
				m_frameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_ambientTexture, 3);
				m_frameBuffer->addTextureAttachment(COLOR_ATTACHMENT, m_tMRF, 4);
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
		if (m_renderDepthTexture) { m_renderDepthTexture->release(); m_renderDepthTexture = nullptr; }
		if (m_tMRF) { m_tMRF->release(); m_tMRF = nullptr; }
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
		addOutputTextureBuffer(m_ambientTexture);
		addOutputTextureBuffer(m_tMRF);

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
		DrawList* drawList = _gameContext->getRenderDrawList();
		_gameContext->getRenderer()->ResetViewport();

		if (m_textureBufferInputs.size() > 0)
		{
			IGPUTexture* depthTexture = m_textureBufferInputs[0];

			// Attach depth texture to frame buffer
			m_frameBuffer->addTextureAttachment(DEPTH_ATTACHMENT, depthTexture);
			m_frameBuffer->setupAttachments();

			_gameContext->getRenderer()->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false, ERendererCompareFunc::LEQUAL, ERendererCompareFunc::ALWAYS, 0, 0, 0>::GetGPUState());
			_gameContext->getRenderer()->Clear(ERenderBufferBit::COLOR_BUFFER_BIT | ERenderBufferBit::STENCIL_BUFFER_BIT);
		}
		else
		{

			// Create the render depth if not supplied
			if (m_renderDepthTexture == nullptr)
			{
				// Depth Texture Buffer
				TextureCreateDesc textureCreateDesc;
				textureCreateDesc.MinFilter = LINEAR;
				textureCreateDesc.MagFilter = LINEAR;
				textureCreateDesc.TextureFormat = TEX_DEPTH24_STENCIL8;
				textureCreateDesc.DataType = UNSIGNED_INT_24_8;
				textureCreateDesc.bGenerateMipMap = false;
				Handle depthRenderBuffer = _gameContext->getRenderZooid()->CreateRenderTexture();
				if (depthRenderBuffer.isValid())
				{
					m_renderDepthTexture = depthRenderBuffer.getObject<IGPUTexture>();
					m_renderDepthTexture->create(textureCreateDesc);
					m_renderDepthTexture->setDebugName("DepthStencilBuffer");

					m_frameBuffer->addTextureAttachment(DEPTH_STENCIL_ATTACHMENT, m_renderDepthTexture);
					m_frameBuffer->setupAttachments();
				}
			}

			_gameContext->getRenderer()->ClearScreen();
		}

		MeshSceneRenderer::Render(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount(), m_shaderChain, m_textureBufferInputs.size() == 0);
		SkinMeshSceneRenderer::Render(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount(), m_shaderSkinChain);
		//TransculentSceneRenderer::Render(drawList->m_transculentRenderGatherer.getRenderInfos(), drawList->m_transculentRenderGatherer.getRenderCount(), m_shaderChain);

		if (m_textureBufferInputs.size() > 0)
		{
			_gameContext->getRenderer()->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());
		}
		return true;
	}

}