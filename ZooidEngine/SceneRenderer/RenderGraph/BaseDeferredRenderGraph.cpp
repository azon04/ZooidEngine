#include "BaseDeferredRenderGraph.h"

#include "SceneRenderer/RenderPass/GBufferRenderPass.h"
#include "SceneRenderer/RenderPass/LightRenderPass.h"
#include "SceneRenderer/RenderPass/ShadowDepthRenderPass.h"
#include "SceneRenderer/RenderPass/ScreenRenderPass.h"
#include "SceneRenderer/RenderPass/DebugRenderPass.h"
#include "SceneRenderer/RenderPass/SSAORenderPass.h"
#include "SceneRenderer/RenderPass/TransculentRenderPass.h"
#include "SceneRenderer/RenderPass/DepthRenderPass.h"
#include "SceneRenderer/RenderPass/HighlightRenderPass.h"

#include "Renderer/IGPUBufferData.h"
#include "Renderer/DrawList.h"
#include "Scene/CameraComponent.h"
#include "Scene/CameraManager.h"
#include "ZEGameContext.h"

namespace ZE
{

	bool g_bUsingDepthPass = true;

	void BaseDeferredRenderGraph::init(GameContext* _gameContext)
	{
		if (g_bUsingDepthPass) { DepthRenderPass::GetInstance()->prepare(_gameContext); }
		GBufferRenderPass::GetInstance()->prepare(_gameContext);
		LightRenderPass::GetInstance()->prepare(_gameContext);
		ShadowDepthRenderPass::GetInstance()->prepare(_gameContext);
		TransculentRenderPass::GetInstance()->prepare(_gameContext);
		ScreenRenderPass::GetInstance()->prepare(_gameContext);
		DebugRenderPass::GetInstance()->prepare(_gameContext);
		SSAORenderPass::GetInstance()->prepare(_gameContext);
		HighlightRenderPass::GetInstance()->prepare(_gameContext);
	}

	void BaseDeferredRenderGraph::release(GameContext* _gameContext)
	{
		if (g_bUsingDepthPass) { DepthRenderPass::GetInstance()->release(_gameContext); }
		GBufferRenderPass::GetInstance()->release(_gameContext);
		LightRenderPass::GetInstance()->release(_gameContext);
		ShadowDepthRenderPass::GetInstance()->release(_gameContext);
		TransculentRenderPass::GetInstance()->release(_gameContext);
		ScreenRenderPass::GetInstance()->release(_gameContext);
		DebugRenderPass::GetInstance()->release(_gameContext);
		SSAORenderPass::GetInstance()->release(_gameContext);
		HighlightRenderPass::GetInstance()->release(_gameContext);
	}

	void BaseDeferredRenderGraph::begin(GameContext* _gameContext)
	{
		Matrix4x4 viewMat;

		ZE::CameraComponent* currentCamera = _gameContext->getCameraManager()->getCurrentCamera();
		if (currentCamera)
		{
			_gameContext->getDrawList()->m_shaderFrameData.setViewMat(_gameContext->getDrawList()->m_viewMat);
			_gameContext->getDrawList()->m_cameraPosition = currentCamera->getWorldPosition();
			_gameContext->getDrawList()->m_cameraDirection = currentCamera->getForwardVector();
			_gameContext->getDrawList()->m_shaderFrameData.setProjectionMat(_gameContext->getDrawList()->m_projectionMat);
			_gameContext->getDrawList()->m_lightData.setViewPos(currentCamera->getWorldPosition());
			_gameContext->getDrawList()->m_mainConstantBuffer->refresh();
		}
	}

	bool BaseDeferredRenderGraph::execute(GameContext* _gameContext)
	{
		if (g_bUsingDepthPass) 
		{
			DepthRenderPass::GetInstance()->begin(_gameContext);
			DepthRenderPass::GetInstance()->Execute(_gameContext);
			DepthRenderPass::GetInstance()->end(_gameContext);
		}

		GBufferRenderPass::GetInstance()->begin(_gameContext);
		if (g_bUsingDepthPass) { GBufferRenderPass::GetInstance()->addInputTextureBuffer(DepthRenderPass::GetInstance()->getTextureOutput(0)); }
		GBufferRenderPass::GetInstance()->Execute(_gameContext);
		GBufferRenderPass::GetInstance()->end(_gameContext);

		ShadowDepthRenderPass::GetInstance()->begin(_gameContext);
		ShadowDepthRenderPass::GetInstance()->Execute(_gameContext);
		ShadowDepthRenderPass::GetInstance()->end(_gameContext);

		//ZCHECK(GBufferRenderPass::GetInstance()->getTextureOuputCount() == 5);

		// SSAO
		SSAORenderPass::GetInstance()->begin(_gameContext);
		SSAORenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(0));
		SSAORenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(1));
		SSAORenderPass::GetInstance()->Execute(_gameContext);
		SSAORenderPass::GetInstance()->end(_gameContext);

		LightRenderPass::GetInstance()->begin(_gameContext);
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(0));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(1));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(2));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(3));
		LightRenderPass::GetInstance()->addInputTextureBuffer(SSAORenderPass::GetInstance()->getTextureOutput(0));
		LightRenderPass::GetInstance()->addInputTextureBuffer(GBufferRenderPass::GetInstance()->getTextureOutput(4));
		LightRenderPass::GetInstance()->addInputFrameBuffer(GBufferRenderPass::GetInstance()->getFrameBufferOutput(0));
		LightRenderPass::GetInstance()->Execute(_gameContext);
		LightRenderPass::GetInstance()->end(_gameContext);

		TransculentRenderPass::GetInstance()->begin(_gameContext);
		TransculentRenderPass::GetInstance()->addInputFrameBuffer(LightRenderPass::GetInstance()->getFrameBufferOutput(0));
		TransculentRenderPass::GetInstance()->Execute(_gameContext);
		TransculentRenderPass::GetInstance()->end(_gameContext);

		HighlightRenderPass::GetInstance()->begin(_gameContext);
		HighlightRenderPass::GetInstance()->addInputFrameBuffer(TransculentRenderPass::GetInstance()->getFrameBufferOutput(0));
		HighlightRenderPass::GetInstance()->Execute(_gameContext);
		HighlightRenderPass::GetInstance()->end(_gameContext);

		DebugRenderPass::GetInstance()->begin(_gameContext);
		DebugRenderPass::GetInstance()->addInputFrameBuffer(TransculentRenderPass::GetInstance()->getFrameBufferOutput(0));
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