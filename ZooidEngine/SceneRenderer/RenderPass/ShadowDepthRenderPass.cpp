#include "ShadowDepthRenderPass.h"
#include "SceneRenderer/ShadowRenderer.h"
#include "Renderer/DrawList.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/IRenderer.h"
#include "ZEGameContext.h"

namespace ZE
{
	ShadowDepthRenderPass::ShadowDepthRenderPass()
	{

	}

	void ShadowDepthRenderPass::prepare(GameContext* _gameContext)
	{

	}

	void ShadowDepthRenderPass::release(GameContext* _gameContext)
	{

	}

	void ShadowDepthRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);
		ShadowDepthRenderer::Reset();
	}

	void ShadowDepthRenderPass::end(GameContext* _gameContext)
	{
		// Refresh light constant buffer after generate the shadow maps
		_gameContext->getDrawList()->m_lightConstantBuffer->refresh();

		// reset viewport
		_gameContext->getRenderer()->ResetViewport();
	}

	bool ShadowDepthRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool ShadowDepthRenderPass::execute_GPU(GameContext* _gameContext)
	{
		DrawList* drawList = _gameContext->getDrawList();
		// For each Light render Shadows
		for (UInt32 iShadowData = 0; iShadowData < drawList->m_lightShadowSize; iShadowData++)
		{
			LightShadowMapData& shadowMapData = drawList->m_lightShadowMapData[iShadowData];
			LightStruct& light = drawList->m_lightData.lights[shadowMapData.lightIndex];

			if (!shadowMapData.dynamicShadowFrameBuffer) { continue; }

			ShadowDepthRenderer::Setup(&shadowMapData, &light, drawList);
			ShadowDepthRenderer::BeginRender();
			ShadowDepthRenderer::Render(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount(), false);
			ShadowDepthRenderer::Render(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount(), true);
			ShadowDepthRenderer::EndRender();
		}

		return true;
	}

}
