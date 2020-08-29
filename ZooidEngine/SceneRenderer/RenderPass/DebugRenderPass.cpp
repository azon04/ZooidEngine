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

		DrawList* drawList = _gameContext->getRenderDrawList();
		MeshSceneRenderer::Render(drawList->m_debugMeshRenderGatherer.getRenderInfos(), drawList->m_debugMeshRenderGatherer.getRenderCount());
		TextSceneRenderer::Render(drawList->m_textSceneRenderGatherer.getRenderInfos(), drawList->m_textSceneRenderGatherer.getRenderCount());
		TextScreenRenderer::Render(drawList->m_textScreenRenderGatherer.getRenderInfos(), drawList->m_textScreenRenderGatherer.getRenderCount());

		if (frameBuffer)
		{
			frameBuffer->unbind();
		}
		
		return true;
	}

}