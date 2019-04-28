#include "SkyboxRenderer.h"

#include "RenderInfo/SkyboxRenderInfo.h"
#include "Renderer/IRenderer.h"
#include "Renderer/IGPUStates.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IShader.h"
#include "Renderer/DrawList.h"

#include "ZEGameContext.h"

namespace ZE
{

	void SkyBoxRenderer::beginRender()
	{
		gGameContext->getRenderer()->SetRenderDepthStencilState(TRenderDepthStencilState<true, false, false, ERendererCompareFunc::LEQUAL, ERendererCompareFunc::ALWAYS, 0, 0xFF, 0xFF>::GetGPUState());
		gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::CULL_NONE, ERenderFillMode::MODE_FILL>::GetGPUState());
	}

	void SkyBoxRenderer::render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		SkyboxRenderInfo* skyboxInfos = static_cast<SkyboxRenderInfo*>(renderInfos);
		for (UInt32 i = 0; i < renderInfoCount; i++)
		{
			SkyboxRenderInfo& currentInfo = skyboxInfos[i];

			ZCHECK(currentInfo.m_shaderChain);
			ZCHECK(currentInfo.m_gpuBufferArray);
			ZCHECK(currentInfo.m_cubeTexture);

			currentInfo.m_shaderChain->bind();

			// Bind frame_data
			gGameContext->getDrawList()->m_mainConstantBuffer->bindAndRefresh();
			currentInfo.m_shaderChain->bindConstantBuffer("frame_data", gGameContext->getDrawList()->m_mainConstantBuffer);

			currentInfo.m_shaderChain->setTexture("skybox", currentInfo.m_cubeTexture, 0);
			currentInfo.m_cubeTexture->bind();

			currentInfo.m_gpuBufferArray->bind();

			gGameContext->getRenderer()->DrawArray(currentInfo.m_renderTopology, 0, currentInfo.drawCount);
		}
	}

	void SkyBoxRenderer::endRender()
	{
		gGameContext->getRenderer()->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());
		gGameContext->getRenderer()->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());
	}

	void SkyBoxRenderer::Render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		SkyBoxRenderer* renderer = GetInstance();
		renderer->beginRender();
		renderer->render(renderInfos, renderInfoCount);
		renderer->endRender();
	}

}