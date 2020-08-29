#include "TransculentRenderPass.h"

#include "Renderer/IRenderer.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/DrawList.h"
#include "Renderer/IGPUBufferData.h"
#include "SceneRenderer/SceneRenderer.h"
#include "ZEGameContext.h"

namespace ZE
{
	TransculentRenderPass::TransculentRenderPass()
	{

	}

	void TransculentRenderPass::prepare(GameContext* _gameContext)
	{

	}

	void TransculentRenderPass::release(GameContext* _gameContext)
	{

	}

	void TransculentRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);
		_gameContext->getRenderer()->ResetViewport();
	}

	void TransculentRenderPass::end(GameContext* _gameContext)
	{
		RenderPass::end(_gameContext);
		if (m_frameBufferInputs.length() > 0)
		{
			addOutputFrameBuffer(m_frameBufferInputs[0]);
		}
	}

	bool TransculentRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool TransculentRenderPass::execute_GPU(GameContext* _gameContext)
	{
		IGPUFrameBuffer* frameBuffer = nullptr;
		if (m_frameBufferInputs.length() > 0)
		{
			frameBuffer = m_frameBufferInputs[0];
			frameBuffer->bind();
		}

		TransculentSceneRenderer::Render(_gameContext->getRenderDrawList()->m_transculentRenderGatherer.getRenderInfos(), _gameContext->getRenderDrawList()->m_transculentRenderGatherer.getRenderCount());

		if (frameBuffer)
		{
			frameBuffer->unbind();
		}

		return true;
	}

}
