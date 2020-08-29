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
	bool g_bDoSceneOcclusion = false;
	bool g_bDoShadowOcclusion = false;

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
			textureDesc.TextureFormat = TEX_DEPTH24_STENCIL8;
			textureDesc.WrapU = CLAMP_TO_BORDER;
			textureDesc.WrapV = CLAMP_TO_BORDER;
			textureDesc.MinFilter = LINEAR;
			textureDesc.MagFilter = LINEAR;
			textureDesc.DataType = UNSIGNED_INT_24_8;
			textureDesc.bGenerateMipMap = false;

			Handle depthTextureHandle = _gameContext->getRenderZooid()->CreateRenderTexture();
			if (depthTextureHandle.isValid())
			{
				m_depthTexture = depthTextureHandle.getObject<IGPUTexture>();
				m_depthTexture->create(textureDesc);
				m_depthTexture->setDebugName("DepthStencilBuffer");
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
				m_frameBuffer->addTextureAttachment(DEPTH_STENCIL_ATTACHMENT, m_depthTexture);
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
		DrawList* drawList = _gameContext->getRenderDrawList();

		_gameContext->getRenderer()->ResetViewport();
		_gameContext->getRenderer()->Clear(ERenderBufferBit::DEPTH_BUFFER_BIT | ERenderBufferBit::STENCIL_BUFFER_BIT);

		MeshSceneRenderer::Render(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount(), m_shaderChain, true);
		SkinMeshSceneRenderer::Render(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount(), m_skinnedShaderChain);

		if (g_bDoSceneOcclusion)
		{
			doOcclusionSceneQueries(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount());
			doOcclusionSceneQueries(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount(), true);
			
			if (g_bDoShadowOcclusion)
			{
				doShadowOcclusionQueries();
			}
		}

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
		gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::CULL_NONE, ERenderFillMode::MODE_FILL>::GetGPUState());

		Matrix4x4 transform;
		Matrix4x4 worldTransform;
		Vector3 extent;
		Vector3 pos;
		bool bUseStencil = false;
		
		for (UInt32 index = 0; index < renderInfoCount; index++)
		{
			if (bUsingSkeleton)
			{
				SkinMeshRenderInfo& skinMesh = skinMeshRenderInfos[index];

				extent = skinMesh.m_boxExtent;
				pos = skinMesh.m_boxLocalPos;
				worldTransform = skinMesh.m_worldTransform;
				bUseStencil = false;
			}
			else
			{
				MeshRenderInfo& currentMesh = meshRenderInfos[index];

				extent = currentMesh.m_boxExtent;
				pos = currentMesh.m_boxLocalPos;
				worldTransform = currentMesh.m_worldTransform;
				bUseStencil = currentMesh.m_outlined;
			}

			// check if scale has any zero
			if (extent.m_x == 0.0f) { extent.m_x = 0.001f; }
			if (extent.m_y == 0.0f) { extent.m_y = 0.001f; }
			if (extent.m_z == 0.0f) { extent.m_z = 0.001f; }

			transform.setScale(extent * 2.0f);
			transform.setPos(pos);
			transform = transform * worldTransform;

			// Bind frame_data
			gGameContext->getRenderDrawList()->m_mainConstantBuffer->bind();
			m_shaderChain->bindConstantBuffer("frame_data", gGameContext->getRenderDrawList()->m_mainConstantBuffer);

			// Create and bind draw data
			IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(transform.m_data, sizeof(Matrix4x4));
			drawBufferData->bind();
			m_shaderChain->bindConstantBuffer("draw_data", drawBufferData);

			renderQueries[index].BeginQuery(gGameContext->getRenderer(), RQ_ANY_SAMPLES_PASSED);
			gGameContext->getRenderer()->DrawArray(ERenderTopologyEnum::TOPOLOGY_TRIANGLE, 0, 36);
			renderQueries[index].EndQuery();
		}
		
		gGameContext->getRenderer()->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());
		gGameContext->getRenderer()->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());

		cubeArray->unbind();
		m_shaderChain->unbind();

		// Flush Command to get the query results
		gGameContext->getRenderer()->FlushCommands();
		gGameContext->getRenderer()->FinishCommands();

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

	void DepthRenderPass::doShadowOcclusionQueries()
	{
		gGameContext->getRenderer()->PushDebugGroup("ShadowOcclusionQueries");

		DrawList* drawList = gGameContext->getRenderDrawList();
		const int shadowMapCount = drawList->m_lightShadowSize;

		// Make Render Queries
		Array<RenderQuery> renderQueries(shadowMapCount);

		// Get the cube buffer
		IGPUBufferArray* cubeArray = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_CUBE);

		m_shaderChain->bind();
		cubeArray->bind();

		// Set Depth State
		gGameContext->getRenderer()->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false, ERendererCompareFunc::LEQUAL, ERendererCompareFunc::ALWAYS, 0, 0, 0>::GetGPUState());
		gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::CULL_NONE, ERenderFillMode::MODE_FILL>::GetGPUState());

		Matrix4x4 transform;
		Matrix4x4 worldTransform;
		Vector3 extent;
		Vector3 pos;

		for (UInt32 index = 0; index < shadowMapCount; index++)
		{
			if (drawList->m_lightShadowMapData[index].cascadeIndex == 0) { continue; } // first cascade index always visible, no need to do this

			transform = drawList->m_lightShadowMapData[index].cullingBoxTransform;

			// Bind frame_data
			gGameContext->getRenderDrawList()->m_mainConstantBuffer->bind();
			m_shaderChain->bindConstantBuffer("frame_data", gGameContext->getRenderDrawList()->m_mainConstantBuffer);

			// Create and bind draw data
			IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(transform.m_data, sizeof(Matrix4x4));
			drawBufferData->bind();
			m_shaderChain->bindConstantBuffer("draw_data", drawBufferData);

			renderQueries[index].BeginQuery(gGameContext->getRenderer(), RQ_ANY_SAMPLES_PASSED);
			gGameContext->getRenderer()->DrawArray(ERenderTopologyEnum::TOPOLOGY_TRIANGLE, 0, 36);
			renderQueries[index].EndQuery();
		}

		gGameContext->getRenderer()->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());
		gGameContext->getRenderer()->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());

		cubeArray->unbind();
		m_shaderChain->unbind();

		// Flush Command to get the query results
		gGameContext->getRenderer()->FlushCommands();
		gGameContext->getRenderer()->FinishCommands();

		for (UInt32 index = 0; index < shadowMapCount; index++)
		{
			LightShadowMapData& lightMapData = drawList->m_lightShadowMapData[index];
			if (lightMapData.cascadeIndex == 0) { continue; } // first cascade index always visible, no need to do this

			if (renderQueries[index].IsResultAvailable())
			{
				lightMapData.bCull = !renderQueries[index].GetBoolResult();
			}
		}

		gGameContext->getRenderer()->PopDebugGroup();
	}

}
