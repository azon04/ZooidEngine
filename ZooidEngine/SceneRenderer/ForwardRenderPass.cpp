#include "ForwardRenderPass.h"

#include "ShadowRenderer.h"
#include "SkyboxRenderer.h"
#include "TextRenderer.h"

#include "Renderer/DrawList.h"
#include "Renderer/IRenderer.h"
#include "ZEGameContext.h"
#include "Scene/CameraComponent.h"
#include "Scene/CameraManager.h"
#include "Renderer/IGPUBufferData.h"

namespace ZE
{
	void ForwardRenderPass::prepare(GameContext* _gameContext)
	{
		
	}

	void ForwardRenderPass::release(GameContext* _gameContext)
	{

	}

	void ForwardRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);

		Matrix4x4 viewMat;

		ZE::CameraComponent* currentCamera = _gameContext->getCameraManager()->getCurrentCamera();
		if (currentCamera)
		{
			currentCamera->getViewMatrix(viewMat);

			_gameContext->getDrawList()->m_shaderFrameData.setViewMat(viewMat);
			_gameContext->getDrawList()->m_cameraPosition = currentCamera->getWorldPosition();
			_gameContext->getDrawList()->m_cameraDirection = currentCamera->getForwardVector();
			_gameContext->getDrawList()->m_shaderFrameData.setProjectionMat(_gameContext->getDrawList()->m_projectionMat);
			_gameContext->getDrawList()->m_lightData.setViewPos(currentCamera->getWorldPosition());
			_gameContext->getDrawList()->m_mainConstantBuffer->refresh();
		}
	}

	void ForwardRenderPass::end(GameContext* _gameContext)
	{

	}

	bool ForwardRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool ForwardRenderPass::execute_GPU(GameContext* _gameContext)
	{
		DrawList* drawList = _gameContext->getDrawList();

		ShadowDepthRenderer::Reset();

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

		// Refresh light constant buffer after generate the shadow maps
		_gameContext->getDrawList()->m_lightConstantBuffer->refresh();

		// reset viewport
		_gameContext->getRenderer()->ResetViewport();

		MeshSceneRenderer::Render(drawList->m_meshRenderGatherer.getRenderInfos(), drawList->m_meshRenderGatherer.getRenderCount());

		SkinMeshSceneRenderer::Render(drawList->m_skinMeshRenderGatherer.getRenderInfos(), drawList->m_skinMeshRenderGatherer.getRenderCount());

		TransculentSceneRenderer::Render(drawList->m_transculentRenderGatherer.getRenderInfos(), drawList->m_transculentRenderGatherer.getRenderCount());
		
		MeshSceneRenderer::Render(drawList->m_debugMeshRenderGatherer.getRenderInfos(), drawList->m_debugMeshRenderGatherer.getRenderCount());

		SkyBoxRenderer::Render(drawList->m_skyboxRenderGatherer.getRenderInfos(), drawList->m_skyboxRenderGatherer.getRenderCount());

		TextSceneRenderer::Render(drawList->m_textSceneRenderGatherer.getRenderInfos(), drawList->m_textSceneRenderGatherer.getRenderCount());
		TextScreenRenderer::Render(drawList->m_textScreenRenderGatherer.getRenderInfos(), drawList->m_textScreenRenderGatherer.getRenderCount());

		return true;
	}

	bool ForwardRenderPass::ExecutePass(GameContext* _gameContext)
	{
		static ForwardRenderPass renderPass;
		renderPass.begin(_gameContext);
		bool result = renderPass.Execute(_gameContext);
		renderPass.end(_gameContext);

		return result;
	}

}
