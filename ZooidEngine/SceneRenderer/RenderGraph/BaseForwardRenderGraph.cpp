#include "BaseForwardRenderGraph.h"

#include "SceneRenderer/ForwardRenderPass.h"
#include "SceneRenderer/RenderPass/ScreenRenderPass.h"


namespace ZE
{

	void BaseForwardRenderGraph::init(GameContext* _gameContext)
	{
		ForwardRenderPass::GetInstance()->prepare(_gameContext);
		ScreenRenderPass::GetInstance()->prepare(_gameContext);
	}

	void BaseForwardRenderGraph::release(GameContext* _gameContext)
	{
		ForwardRenderPass::GetInstance()->release(_gameContext);
		ScreenRenderPass::GetInstance()->release(_gameContext);
	}

	void BaseForwardRenderGraph::begin(GameContext* _gameContext)
	{

	}

	bool BaseForwardRenderGraph::execute(GameContext* _gameContext)
	{
		ForwardRenderPass::GetInstance()->begin(_gameContext);
		ForwardRenderPass::GetInstance()->Execute(_gameContext);
		ForwardRenderPass::GetInstance()->end(_gameContext);

		ScreenRenderPass::GetInstance()->begin(_gameContext);
		ScreenRenderPass::GetInstance()->addInputTextureBuffer(ForwardRenderPass::GetInstance()->getTextureOutput(0));
		ScreenRenderPass::GetInstance()->Execute(_gameContext);
		ScreenRenderPass::GetInstance()->end(_gameContext);

		return true;
	}

	void BaseForwardRenderGraph::end(GameContext* _gameContext)
	{

	}

}