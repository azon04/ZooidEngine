#include "ShadowRenderer.h"

#include "Memory/Handle.h"
#include "Renderer/DrawList.h"
#include "Scene/Light/LightComponent.h"
#include "Math/MathOps.h"
#include "ZEGameContext.h"
#include "Renderer/Structs.h"
#include "Renderer/IRenderer.h"
#include "RenderInfo/MeshRenderInfo.h"
#include "RenderInfo/SkinMeshRenderInfo.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IShader.h"
#include "Renderer/IGPUStates.h"
#include "ResourceManagers/BufferManager.h"

namespace ZE
{
	void ShadowDepthRenderer::setupShadowMapData(LightShadowMapData* shadowMapData)
	{
		CascadeShadowData* casecadeData = nullptr;
		LightData& globalLightData = gGameContext->getDrawList()->m_lightData;
		if (shadowMapData->cascadeIndex >= 0)
		{
			casecadeData = &(gGameContext->getDrawList()->m_lightData.cascadeShadowData[shadowMapData->cascadeIndex]);
		}

		m_shaderChain = shadowMapData->normalShaderChain;
		m_skinnedShaderChain = shadowMapData->skinnedShaderChain;
		m_currentFrameBuffer = shadowMapData->dynamicShadowFrameBuffer;

		UInt32 shadowTextureIndex = 0;
		while (m_currentLight->shadowMapIndices[shadowTextureIndex++] != -1) {}
		shadowTextureIndex--;

		if (shadowMapData->staticShadowTexture)
		{
			UInt32 index = m_shadowTextures.size();
			m_currentLight->shadowMapIndices[shadowTextureIndex++] = index;
			globalLightData.shadowData[index].setViewProjMatrix(shadowMapData->view * shadowMapData->projection);
			m_shadowTextures.push_back(shadowMapData->staticShadowTexture);
		}

		if (shadowMapData->dynamicShadowTexture)
		{
			UInt32 index = m_shadowTextures.size();
			if (casecadeData)
			{
				casecadeData->shadowMapIndex = index;
			}
			else
			{
				m_currentLight->shadowMapIndices[shadowTextureIndex++] = index;
			}
			globalLightData.shadowData[index].setViewProjMatrix(shadowMapData->view * shadowMapData->projection);
			m_shadowTextures.push_back(shadowMapData->dynamicShadowTexture);
		}

		if (!m_currentFrameBuffer) { return; }

		m_shaderData.setViewMat(shadowMapData->view);
		m_shaderData.setProjectionMat(shadowMapData->projection);
		m_shaderConstantBuffer->refresh();
	}

	void ShadowDepthRenderer::reset()
	{
		m_shadowTextures.clear();
	}

	void ShadowDepthRenderer::beginRender()
	{
		if (!m_currentFrameBuffer) { return; }

		m_currentFrameBuffer->bind();
		gGameContext->getRenderer()->Clear(ERenderBufferBit::DEPTH_BUFFER_BIT);

		// Set Face Culling Front face to fix peter panning
		gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<
			EFaceFrontOrder::CCW,
			ECullFace::FRONT,
			ERenderFillMode::MODE_FILL>::GetGPUState());
	}

	void ShadowDepthRenderer::render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		if (!m_currentFrameBuffer) { return; }

		IShaderChain* shaderChain = m_bRenderSkinMesh ? m_skinnedShaderChain : m_shaderChain;

		shaderChain->bind();

		// Bind shader_data
		m_shaderConstantBuffer->bind();
		shaderChain->bindConstantBuffer("frame_data", m_shaderConstantBuffer);

		if (m_bRenderSkinMesh)
		{
			SkinMeshRenderInfo* meshRenderInfos = static_cast<SkinMeshRenderInfo*>(renderInfos);

			//ZELOG(LOG_ENGINE, Log, "Shadow Count: %d", renderInfoCount);

			for (UInt32 index = 0; index < renderInfoCount; index++)
			{
				SkinMeshRenderInfo& currentMesh = meshRenderInfos[index];

				if (!currentMesh.m_castShadow) { continue; }

				ZCHECK(currentMesh.m_gpuBufferArray);
				
				// Bind bone_data
				currentMesh.m_skinJointData->bind();
				shaderChain->bindConstantBuffer("bone_mats_block", currentMesh.m_skinJointData);
				
				// Create and bind draw data
				IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(currentMesh.m_worldTransform.m_data, sizeof(Matrix4x4));
				drawBufferData->bind();
				shaderChain->bindConstantBuffer("draw_data", drawBufferData);

				currentMesh.m_gpuBufferArray->bind();

				gGameContext->getRenderer()->DrawArray(currentMesh.m_renderTopology, 0, currentMesh.drawCount);
			}
		}
		else
		{
			MeshRenderInfo* meshRenderInfos = static_cast<MeshRenderInfo*>(renderInfos);

			for (UInt32 index = 0; index < renderInfoCount; index++)
			{
				MeshRenderInfo& currentMesh = meshRenderInfos[index];

				if (!currentMesh.m_castShadow) { continue; }

				ZCHECK(currentMesh.m_gpuBufferArray);

				// Create and bind draw data
				IGPUBufferData* drawBufferData = BufferManager::getInstance()->getOrCreateDrawBuffer(currentMesh.m_worldTransform.m_data, sizeof(Matrix4x4));
				drawBufferData->bind();
				shaderChain->bindConstantBuffer("draw_data", drawBufferData);

				currentMesh.m_gpuBufferArray->bind();

				gGameContext->getRenderer()->DrawArray(currentMesh.m_renderTopology, 0, currentMesh.drawCount);
			}
		}

		shaderChain->unbind();

	}

	void ShadowDepthRenderer::endRender()
	{
		if (!m_currentFrameBuffer) { return; }
		m_currentFrameBuffer->unbind();

		gGameContext->getRenderer()->SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());
	}

	void ShadowDepthRenderer::Setup(LightShadowMapData* shadowMapData, LightStruct* lightData, DrawList* drawList)
	{
		getInstance()->m_currentLight = lightData;
		getInstance()->setupShadowMapData(shadowMapData);
	}

	void ShadowDepthRenderer::Render(RenderInfo* renderInfos, UInt32 renderInfoCount, bool bSkinMesh)
	{
		getInstance()->m_bRenderSkinMesh = bSkinMesh;
		getInstance()->render(renderInfos, renderInfoCount);
	}

	void ShadowDepthRenderer::BindShadowTextures(DrawList* _drawList, IShaderChain* shaderChain, UInt32 textureIndexOffset)
	{
		ShadowDepthRenderer* renderer = getInstance();
		char buffer[25];
		for (Int32 i = 0; i < renderer->m_shadowTextures.size(); i++)
		{
			StringFunc::PrintToString(buffer, 25, "shadowMaps[%d]", i);
			shaderChain->setTexture(buffer, renderer->m_shadowTextures[i], textureIndexOffset + i);
			renderer->m_shadowTextures[i]->bind();
		}
	}

	ZE::ShadowDepthRenderer* ShadowDepthRenderer::getInstance()
	{
		if (!s_shadowRenderer)
		{
			Handle hShadowRenderer("ShadowDepthRenderer", sizeof(ShadowDepthRenderer));
			s_shadowRenderer = new(hShadowRenderer) ShadowDepthRenderer;

			s_shadowRenderer->m_shaderConstantBuffer = BufferManager::getInstance()->createConstantBuffer(&(s_shadowRenderer->m_shaderData), sizeof(ShaderData), CONSTANT_BUFFER_SHADER_DATA_INDEX);
		}

		return s_shadowRenderer;
	}

	ZE::ShadowDepthRenderer* ShadowDepthRenderer::s_shadowRenderer = nullptr;

}