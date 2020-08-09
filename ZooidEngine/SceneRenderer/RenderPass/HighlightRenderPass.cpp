#include "HighlightRenderPass.h"

#include "ZEGameContext.h"
#include "Renderer/DrawList.h"
#include "Renderer/IRenderer.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/BufferManager.h"
#include "Logging/Log.h"

namespace ZE
{
	void HighlightRenderPass::prepare(GameContext* _gameContext)
	{
#if HIGHLIGHT_ALG == HIGHLIGHT_SHELL
		m_shader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/DefaultHighlightShell.vs", "ZooidEngine/Shaders/DefaultHighlightSimple.frag", nullptr, nullptr);
		ZCHECK(m_shader);
#endif
	}

	void HighlightRenderPass::release(GameContext* _gameContext)
	{

	}

	bool HighlightRenderPass::execute_CPU(GameContext* _gameContext)
	{
		return true;
	}

	bool HighlightRenderPass::execute_GPU(GameContext* _gameContext)
	{
		DrawList* drawList = _gameContext->getDrawList();
		IRenderer* renderer = _gameContext->getRenderer();
		IShaderChain* shaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_3D_HIGHLIGHT);

#if HIGHLIGHT_ALG == HIGHLIGHT_SHELL
		shaderChain = m_shader;
#endif

		IGPUFrameBuffer* frameBuffer = nullptr;
		if (m_frameBufferInputs.size() > 0)
		{
			frameBuffer = m_frameBufferInputs[0];
			frameBuffer->bind();
		}

		renderer->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::FRONT, ERenderFillMode::MODE_FILL>::GetGPUState());
		renderer->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false>::GetGPUState());

		if (!shaderChain)
		{
			ZELOG(LOG_RENDERING, Warning, "Shader for highlight can't be found");
			return false;
		}
		
		UInt32 meshCount = drawList->m_highligtRenderGatherer.getRenderCount();
		MeshRenderInfo* meshRenderInfos = static_cast<MeshRenderInfo*>(drawList->m_highligtRenderGatherer.getRenderInfos());

		shaderChain->bind();

		// bind frame_data
		drawList->m_mainConstantBuffer->bind();
		shaderChain->bindConstantBuffer("frame_data", drawList->m_mainConstantBuffer);

		// set highlight color
		shaderChain->setVec3("highlightColor", m_highLightColor);


		for (UInt32 index = 0; index < meshCount; index++)
		{
			MeshRenderInfo& meshRenderInfo = meshRenderInfos[index];

			// create and bind draw data
			Matrix4x4 adjustedTransform = meshRenderInfo.m_worldTransform;
#if HIGHLIGHT_ALG == HIGHLIGHT_NAIVE
			Vector3 adjustedPos = meshRenderInfo.m_boxLocalPos * adjustedTransform;
			Vector3 cameraDirection = drawList->m_cameraPosition -  adjustedPos;
			adjustedTransform.setPos(adjustedTransform.getPos() + 0.01 * cameraDirection);
#endif
			IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(adjustedTransform.m_data, sizeof(Matrix4x4));
			drawBufferData->bind();
			shaderChain->bindConstantBuffer("draw_data", drawBufferData);

			meshRenderInfo.m_gpuBufferArray->bind();
			if (meshRenderInfo.m_gpuBufferArray->isUsingIndexBuffer())
			{
				renderer->DrawIndexed(meshRenderInfo.m_renderTopology, 4, meshRenderInfo.drawCount, nullptr);
			}
			else
			{
				renderer->DrawArray(meshRenderInfo.m_renderTopology, 0, meshRenderInfo.drawCount);
			}
			meshRenderInfo.m_gpuBufferArray->unbind();
		}

		shaderChain->unbind();

		if (frameBuffer)
		{
			frameBuffer->unbind();
		}

		renderer->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());
		renderer->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());

		return true;
	}

}
