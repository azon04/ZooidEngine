#include "DebugRenderPass.h"

#include "Renderer/IRenderer.h"
#include "SceneRenderer/SceneRenderer.h"
#include "Renderer/DrawList.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "ZEGameContext.h"

namespace ZE
{

	DebugRenderPass::DebugRenderPass()
	{

	}

	void DebugRenderPass::prepare(GameContext* _gameContext)
	{

	}

	void DebugRenderPass::release(GameContext* _gameContext)
	{

	}

	void DebugRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);
		_gameContext->getRenderer()->ResetViewport();
	}

	void DebugRenderPass::end(GameContext* _gameContext)
	{
		if (m_frameBufferInputs.length() > 0)
		{
			addOutputFrameBuffer(m_frameBufferInputs[0]);
		}
	}

	bool DebugRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool DebugRenderPass::execute_GPU(GameContext* _gameContext)
	{
		IGPUFrameBuffer* frameBuffer = nullptr;
		if (m_frameBufferInputs.length() > 0)
		{
			frameBuffer = m_frameBufferInputs[0];
			frameBuffer->bind();
		}

		MeshSceneRenderer::Render(_gameContext->getDrawList()->m_debugMeshRenderGatherer.getRenderInfos(), _gameContext->getDrawList()->m_debugMeshRenderGatherer.getRenderCount());
		
		if (frameBuffer)
		{
			frameBuffer->unbind();
		}
		
		return true;
	}

}