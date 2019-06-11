#include "ScreenRenderPass.h"
#include "ResourceManagers\ShaderManager.h"
#include "ResourceManagers\BufferManager.h"
#include "Renderer\IGPUTexture.h"
#include "Renderer\IRenderer.h"
#include "ZEGameContext.h"

namespace ZE
{

	ScreenRenderPass::ScreenRenderPass()
		: m_shaderChain(nullptr)
	{

	}

	void ScreenRenderPass::prepare(GameContext* _gameContext)
	{
		if (!m_shaderChain)
		{
			m_shaderChain = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DeferredShading/ScreenRenderShader.vs", "ZooidEngine/Shaders/DeferredShading/ScreenRenderShader.frag", nullptr, nullptr);
		}
	}

	void ScreenRenderPass::release(GameContext* _gameContext)
	{

	}

	void ScreenRenderPass::begin(GameContext* _gameContext)
	{
		RenderPass::begin(_gameContext);
		_gameContext->getRenderer()->ResetViewport();
		_gameContext->getRenderer()->ClearScreen();
	}

	void ScreenRenderPass::end(GameContext* _gameContext)
	{

	}

	bool ScreenRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool ScreenRenderPass::execute_GPU(GameContext* _gameContext)
	{
		IGPUBufferArray* quadVBO = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);
		
		ZCHECK(m_textureBufferInputs.length() >= 1);

		IGPUTexture* finalTexture = m_textureBufferInputs[0];

		m_shaderChain->bind();

		m_shaderChain->setTexture("finalTexture", finalTexture, 0);
		finalTexture->bind();

		m_shaderChain->setInt("hdrToneMap", 1);
		m_shaderChain->setFloat("exposure", 1.0f);

		_gameContext->getRenderer()->DrawBufferArray(TOPOLOGY_TRIANGLE, quadVBO, 6);
		
		finalTexture->unbind();
		m_shaderChain->unbind();

		return true;
	}

}