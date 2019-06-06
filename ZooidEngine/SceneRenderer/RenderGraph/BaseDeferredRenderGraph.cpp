#include "BaseDeferredRenderGraph.h"

#include "SceneRenderer/RenderPass/GBufferRenderPass.h"
#include "SceneRenderer/RenderPass/LightRenderPass.h"
#include "SceneRenderer/RenderPass/ShadowDepthRenderPass.h"
#include "SceneRenderer/RenderPass/ScreenRenderPass.h"
#include "SceneRenderer/RenderPass/DebugRenderPass.h"

#include "Renderer/IGPUBufferData.h"
#include "Renderer/DrawList.h"
#include "Scene/CameraComponent.h"
#include "Scene/CameraManager.h"
#include "ZEGameContext.h"

namespace ZE
{

	void BaseDeferredRenderGraph::init(GameContext* _gameContext)
	{
		GBufferRenderPass::GetInstance()->prepare(_gameContext);
		LightRenderPass::GetInstance()->prepare(_gameContext);
		ShadowDepthRenderPass::GetInstance()->prepare(_gameContext);
		ScreenRenderPass::GetInstance()->prepare(_gameContext);
		DebugRenderPass::GetInstance()->prepare(_gameContext);
	}

	void BaseDeferredRenderGraph::release(GameContext* _gameContext)
	{
		GBufferRenderPass::GetInstance()->release(_gameContext);
		LightRenderPass::GetInstance()->release(_gameContext);
		ShadowDepthRenderPass::GetInstance()->release(_gameContext);
		ScreenRenderPass::GetInstance()->release(_gameContext);
		DebugRenderPass::GetInstance()->release(_gameContext);
	}

	void BaseDeferredRenderGraph::begin(GameContext* _gameContext)
	{
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

	bool BaseDeferredRenderGraph::execute(GameContext* _gameContext)
	{
		GBufferRenderPass::GetInstance()->begin(_gameContext);
		GBufferRenderPass::GetInstance()->Execute(_gameContext);
		GBufferRenderPass::GetInstance()->end(_gameContext);

		ShadowDepthRenderPass::GetInstance()->begin(_gameContext);
		ShadowDepthRenderPass::GetInstance()->Execute(_gameContext);
		ShadowDepthRenderPass::GetInstance()->end(_gameContext);

		ZCHECK(GBufferRenderPass::GetInstance()->getTextureOuputCount() == 5);

		LightRenderPass::GetInstance()->begin(_gameContext);
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(0));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(1));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(2));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(3));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(4));
		LightRenderPass::GetInstance()->addInputFrameBuffer(GBufferRenderPass::GetInstance()->getFrameBufferOutput(0));
		LightRenderPass::GetInstance()->Execute(_gameContext);
		LightRenderPass::GetInstance()->end(_gameContext);

		DebugRenderPass::GetInstance()->begin(_gameContext);
		DebugRenderPass::GetInstance()->addInputFrameBuffer(LightRenderPass::GetInstance()->getFrameBufferOutput(0));
		DebugRenderPass::GetInstance()->Execute(_gameContext);
		DebugRenderPass::GetInstance()->end(_gameContext);

		ScreenRenderPass::GetInstance()->begin(_gameContext);
		ScreenRenderPass::GetInstance()->addInputTextureBuffer(LightRenderPass::GetInstance()->getTextureOutput(0));
		ScreenRenderPass::GetInstance()->Execute(_gameContext);
		ScreenRenderPass::GetInstance()->end(_gameContext);

		
		return true;
	}

	void BaseDeferredRenderGraph::end(GameContext* _gameContext)
	{

	}

}