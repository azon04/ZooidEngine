#include "TextRenderer.h"

#include "RenderInfo/TextRenderInfo.h"
#include "Renderer/IShader.h"
#include "Renderer/DrawList.h"
#include "Renderer/IRenderer.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPUStates.h"
#include "ResourceManagers/BufferManager.h"
#include "ZEGameContext.h"

namespace ZE
{

	void TextSceneRenderer::beginRender()
	{
		gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW,
			ECullFace::CULL_NONE,
			ERenderFillMode::MODE_FILL>::GetGPUState());
		gGameContext->getRenderer()->SetRenderBlendState(TRenderBlendState<true, ERendererBlendFactor::SRC_ALPHA, ERendererBlendFactor::ONE_MINUS_SRC_ALPHA, 0>::GetGPUState());
	}

	void TextSceneRenderer::render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		TextRenderInfo* textRenderInfos = static_cast<TextRenderInfo*>(renderInfos);
		for (UInt32 i = 0; i < renderInfoCount; i++)
		{
			TextRenderInfo& currentInfo = textRenderInfos[i];

			ZCHECK(currentInfo.m_shaderChain);
			ZCHECK(currentInfo.m_gpuBufferArray);

			currentInfo.m_shaderChain->bind();

			// Bind Frame data
			gGameContext->getDrawList()->m_mainConstantBuffer->bindAndRefresh();
			currentInfo.m_shaderChain->bindConstantBuffer("frame_data", gGameContext->getDrawList()->m_mainConstantBuffer);

			// Bind light_data
			gGameContext->getDrawList()->m_lightConstantBuffer->bindAndRefresh();
			currentInfo.m_shaderChain->bindConstantBuffer("light_data", gGameContext->getDrawList()->m_lightConstantBuffer);

			// Create and bind draw data
			IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(currentInfo.m_worldTransform.m_data, sizeof(Matrix4x4));
			drawBufferData->bind();
			currentInfo.m_shaderChain->bindConstantBuffer("draw_data", drawBufferData);

			if (currentInfo.m_fontTexture)
			{
				currentInfo.m_shaderChain->setTexture("fontTexture", currentInfo.m_fontTexture, 0);
				currentInfo.m_fontTexture->bind();
			}

			currentInfo.m_shaderChain->setVec3("color", currentInfo.m_color);

			currentInfo.m_gpuBufferArray->bind();

			gGameContext->getRenderer()->DrawArray(currentInfo.m_renderTopology, 0, currentInfo.drawCount);
		}
	}

	void TextSceneRenderer::endRender()
	{
		gGameContext->getRenderer()->SetRenderBlendState(DefaultRenderBlendState::GetGPUState());
		gGameContext->getRenderer()->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());
	}

	void TextSceneRenderer::Render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		TextSceneRenderer* renderer = GetInstance();
		renderer->beginRender();

		renderer->render(renderInfos, renderInfoCount);

		renderer->endRender();
	}

	void TextScreenRenderer::beginRender()
	{
		gGameContext->getRenderer()->SetRenderBlendState(TRenderBlendState<true, ERendererBlendFactor::SRC_ALPHA, ERendererBlendFactor::ONE_MINUS_SRC_ALPHA, 0>::GetGPUState());
	}

	void TextScreenRenderer::render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		TextRenderInfo* textRenderInfos = static_cast<TextRenderInfo*>(renderInfos);
		for (UInt32 i = 0; i < renderInfoCount; i++)
		{
			TextRenderInfo& currentInfo = textRenderInfos[i];

			ZCHECK(currentInfo.m_shaderChain);
			ZCHECK(currentInfo.m_gpuBufferArray);

			currentInfo.m_shaderChain->bind();
			currentInfo.m_shaderChain->setMat("model", currentInfo.m_worldTransform);
			currentInfo.m_shaderChain->setFloat("screenHeight", gGameContext->getRenderer()->GetHeight());
			currentInfo.m_shaderChain->setFloat("screenWidth", gGameContext->getRenderer()->GetWidth());

			if (currentInfo.m_fontTexture)
			{
				currentInfo.m_shaderChain->setTexture("fontTexture", currentInfo.m_fontTexture, 0);
				currentInfo.m_fontTexture->bind();
			}

			currentInfo.m_shaderChain->setVec3("color", currentInfo.m_color);

			currentInfo.m_gpuBufferArray->bind();

			gGameContext->getRenderer()->DrawArray(currentInfo.m_renderTopology, 0, currentInfo.drawCount);
		}
	}

	void TextScreenRenderer::endRender()
	{
		gGameContext->getRenderer()->SetRenderBlendState(DefaultRenderBlendState::GetGPUState());
	}

	void TextScreenRenderer::Render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		TextScreenRenderer* renderer = GetInstance();
		renderer->beginRender();
		renderer->render(renderInfos, renderInfoCount);
		renderer->endRender();
	}

}