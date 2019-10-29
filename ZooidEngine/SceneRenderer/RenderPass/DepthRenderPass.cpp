#include "DepthRenderPass.h"

#include "ResourceManagers/ShaderManager.h"
#include "Resources/Texture.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUTexture.h"
#include "SceneRenderer/SceneRenderer.h"
#include "Math/Vector4.h"
#include "Renderer/DrawList.h"

namespace ZE
{
	DepthRenderPass::DepthRenderPass()
	{
		m_shaderChain = nullptr;
		m_skinnedShaderChain = nullptr;
		m_depthTexture = nullptr;
		m_frameBuffer = nullptr;
	}

	void DepthRenderPass::prepare(GameContext* _gameContext)
	{
		ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());

		m_shaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_DEPTH_ONLY);
		m_skinnedShaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_DEPTH_ONLY_SKINNED);

		if (!m_depthTexture)
		{
			TextureCreateDesc textureDesc;
			textureDesc.Width = _gameContext->getRenderer()->GetWidth();
			textureDesc.Height = _gameContext->getRenderer()->GetHeight();
			textureDesc.Channel = 1;
			textureDesc.TextureFormat = TEX_DEPTH;
			textureDesc.WrapU = CLAMP_TO_BORDER;
			textureDesc.WrapV = CLAMP_TO_BORDER;
			textureDesc.MinFilter = LINEAR;
			textureDesc.MagFilter = LINEAR;
			textureDesc.DataType = FLOAT;
			textureDesc.bGenerateMipMap = false;
			textureDesc.BorderColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);

			Handle depthTextureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (depthTextureHandle.isValid())
			{
				m_depthTexture = depthTextureHandle.getObject<IGPUTexture>();
				m_depthTexture->create(textureDesc);
			}
		}

		if (!m_frameBuffer)
		{
			// Create Frame buffer
			Handle fbHandle = _gameContext->getRenderZooid()->CreateFrameBuffer();
			if (fbHandle.isValid())
			{
				m_frameBuffer = fbHandle.getObject<IGPUFrameBuffer>();
				m_frameBuffer->bind();
				m_frameBuffer->addTextureAttachment(DEPTH_ATTACHMENT, m_depthTexture);
				m_frameBuffer->setupAttachments();
				m_frameBuffer->unbind();
			}
		}
	}

	void DepthRenderPass::release(GameContext* _gameContext)
	{
		if (m_frameBuffer) { m_frameBuffer->release(); m_frameBuffer = nullptr; }
		if (m_depthTexture) { m_depthTexture->release(); m_depthTexture = nullptr; }
	}

	void DepthRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);

		ZCHECK(m_frameBuffer);

		m_frameBuffer->bind();
	}

	void DepthRenderPass::end(GameContext* _gameContext)
	{
		RenderPass::end(_gameContext);
		
		m_frameBuffer->unbind();

		// Add output
		addOutputTextureBuffer(m_depthTexture);
	}

	bool DepthRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool DepthRenderPass::execute_GPU(GameContext* _gameContext)
	{
		DrawList* drawList = _gameContext->getDrawList();

		_gameContext->getRenderer()->ResetViewport();
		_gameContext->getRenderer()->Clear(ERenderBufferBit::DEPTH_BUFFER_BIT);

		MeshSceneRenderer::Render(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount(), m_shaderChain);
		SkinMeshSceneRenderer::Render(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount(), m_skinnedShaderChain);

		return true;
	}

}
