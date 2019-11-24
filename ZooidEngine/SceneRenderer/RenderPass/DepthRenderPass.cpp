#include "DepthRenderPass.h"

#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/BufferManager.h"
#include "Resources/Texture.h"
#include "Renderer/IRenderer.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/RenderQuery.h"
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
			textureDesc.Width = (UInt32)_gameContext->getRenderer()->GetWidth();
			textureDesc.Height = (UInt32)_gameContext->getRenderer()->GetHeight();
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

		doOcclusionSceneQueries(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount());
		doOcclusionSceneQueries(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount(), true);

		return true;
	}

	void DepthRenderPass::doOcclusionSceneQueries(RenderInfo* renderInfos, UInt32 renderInfoCount, bool bUsingSkeleton)
	{
		if (renderInfoCount == 0) { return; }
		
		gGameContext->getRenderer()->PushDebugGroup("SceneOcclusionQueries");

		MeshRenderInfo* meshRenderInfos = static_cast<MeshRenderInfo*>(renderInfos);
		SkinMeshRenderInfo* skinMeshRenderInfos = static_cast<SkinMeshRenderInfo*>(renderInfos);

		// Make Render Queries
		Array<RenderQuery> renderQueries(renderInfoCount);

		// Get the cube buffer
		IGPUBufferArray* cubeArray = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_CUBE);

		m_shaderChain->bind();
		cubeArray->bind();

		// Set Depth State
		gGameContext->getRenderer()->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false, ERendererCompareFunc::LEQUAL, ERendererCompareFunc::ALWAYS, 0, 0, 0>::GetGPUState());

		for (UInt32 index = 0; index < renderInfoCount; index++)
		{
			Matrix4x4 transform;

			if (bUsingSkeleton)
			{
				SkinMeshRenderInfo& skinMesh = skinMeshRenderInfos[index];

				transform.setScale(skinMesh.m_boxExtent * 2.0f);
				transform.setPos(skinMesh.m_boxLocalPos);
				transform = transform * skinMesh.m_worldTransform;
			}
			else
			{
				MeshRenderInfo& currentMesh = meshRenderInfos[index];

				transform.setScale(currentMesh.m_boxExtent * 2.0f);
				transform.setPos(currentMesh.m_boxLocalPos);
				transform = transform * currentMesh.m_worldTransform;
			}

			// Bind frame_data
			gGameContext->getDrawList()->m_mainConstantBuffer->bind();
			m_shaderChain->bindConstantBuffer("frame_data", gGameContext->getDrawList()->m_mainConstantBuffer);

			// Create and bind draw data
			IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(transform.m_data, sizeof(Matrix4x4));
			drawBufferData->bind();
			m_shaderChain->bindConstantBuffer("draw_data", drawBufferData);

			renderQueries[index].BeginQuery(gGameContext->getRenderer(), RQ_ANY_SAMPLES_PASSED);
			gGameContext->getRenderer()->DrawArray(ERenderTopologyEnum::TOPOLOGY_TRIANGLE, 0, 36);
			renderQueries[index].EndQuery();
		}
		
		gGameContext->getRenderer()->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());

		cubeArray->unbind();
		m_shaderChain->unbind();

		// Flush Command to get the query results
		gGameContext->getRenderer()->FlushCommands();

		for (UInt32 index = 0; index < renderInfoCount; index++)
		{
			MeshRenderInfo& currentMesh = meshRenderInfos[index];
			if (renderQueries[index].IsResultAvailable())
			{
				currentMesh.m_bCulled = !renderQueries[index].GetBoolResult();
			}
		}

		gGameContext->getRenderer()->PopDebugGroup();
	}

}
