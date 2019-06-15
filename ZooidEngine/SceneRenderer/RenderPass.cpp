#include "RenderPass.h"

#include "Renderer/IRenderer.h"
#include "ZEGameContext.h"

namespace ZE
{

	void RenderPass::begin(GameContext* _gameContext)
	{
		m_textureBufferInputs.clear();
		m_textureBufferOutputs.clear();
		m_frameBufferInputs.clear();
		m_frameBufferOuputs.clear();
		_gameContext->getRenderer()->PushDebugGroup(getRenderPassName());
	}

	void RenderPass::end(GameContext* _gameContext)
	{
		_gameContext->getRenderer()->PopDebugGroup();
	}

}