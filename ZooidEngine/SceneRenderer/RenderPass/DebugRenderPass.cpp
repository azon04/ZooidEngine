#include "DebugRenderPass.h"

#include "Renderer/IRenderer.h"
#include "SceneRenderer/SceneRenderer.h"
#include "SceneRenderer/TextRenderer.h"
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
		RenderPass::end(_gameContext);
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

		_gameContext->getRenderer()->ResetViewport();

		MeshSceneRenderer::Render(_gameContext->getDrawList()->m_debugMeshRenderGatherer.getRenderInfos(), _gameContext->getDrawList()->m_debugMeshRenderGatherer.getRenderCount());
		TextSceneRenderer::Render(_gameContext->getDrawList()->m_textSceneRenderGatherer.getRenderInfos(), _gameContext->getDrawList()->m_textSceneRenderGatherer.getRenderCount());
		TextScreenRenderer::Render(_gameContext->getDrawList()->m_textScreenRenderGatherer.getRenderInfos(), _gameContext->getDrawList()->m_textScreenRenderGatherer.getRenderCount());

		if (frameBuffer)
		{
			frameBuffer->unbind();
		}
		
		return true;
	}

}