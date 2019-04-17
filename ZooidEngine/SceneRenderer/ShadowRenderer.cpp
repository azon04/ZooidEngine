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

	void ShadowDepthRenderer::setupLightData(LightStruct* lightData, DrawList* drawList)
	{
		Matrix4x4 view;
		Matrix4x4 proj;

		if (lightData->Type == LightType::DIRECTIONAL_LIGHT)
		{
			Vector3 zAxis = lightData->getDirection();
			Vector3 xAxis = zAxis ^ Vector3(0.0f, 1.0f, 0.0f);
			if (xAxis.lengthSquare() == 0.0f)
			{
				xAxis = zAxis ^ Vector3(0.0f, 0.0f, 1.0f);
			}

			Vector3 yAxis = xAxis ^ zAxis;

			zAxis = zAxis * -1.0f;

			view.setU(xAxis);
			view.setV(yAxis);
			view.setN(zAxis);

			view = view.transpose();

			Float32 mostRight = 0.0f;
			Float32 mostLeft = 0.0f;
			Float32 mostTop = 0.0f;
			Float32 mostBottom = 0.0f;
			Float32 mostNear = 0.0f;
			Float32 mostFar = 0.0f;

			ViewFustrum& mainFustrum = drawList->m_viewFustrum;

			for (UInt32 i = 0; i < 8; i++)
			{
				Vector3 fustrumPoint = mainFustrum.getFustrumPoint((EFustrumPoint)(i));
				
				float right = xAxis | fustrumPoint;
				float top = yAxis | fustrumPoint;
				float nearD = zAxis | fustrumPoint;

				if (i == 0)
				{
					mostRight = mostLeft = right;
					mostTop = mostBottom = top;
					mostNear = mostFar = nearD;
				}
				else
				{
					if (mostRight < right) { mostRight = right; }
					if (mostLeft > right) { mostLeft = right; }
					if (mostTop < top) { mostTop = top; }
					if (mostBottom > top) { mostBottom = top; }
					if (mostNear < nearD) { mostNear = nearD; }
					if (mostFar > nearD) { mostFar = nearD; }
				}
			}

			// Test Object bounding; make the light fustrum smaller
			{
				float obMostRight = 0.0f;
				float obMostLeft = 0.0f;
				float obMostTop = 0.0f;
				float obMostBottom = 0.0f;
				float obMostNear = 0.0f;

				Vector3 vertices[2] = { drawList->m_objectsBounding.m_min, drawList->m_objectsBounding.m_max };
				for (int i = 0; i < 2; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						for (int k = 0; k < 2; k++)
						{
							Vector3 vertex(vertices[i].m_x, vertices[j].m_y, vertices[k].m_z);

							float right = xAxis | vertex;
							float top = yAxis | vertex;
							float nearD = zAxis | vertex;

							if (i == 0 && j == 0 && k == 0)
							{
								obMostRight = obMostLeft = right;
								obMostTop = obMostBottom = top;
							}
							else
							{
								if (obMostRight < right) { obMostRight = right; }
								if (obMostLeft > right) { obMostLeft = right; }
								if (obMostTop < top) { obMostTop = top; }
								if (obMostBottom > top) { obMostBottom = top; }
								if (obMostNear < nearD) { obMostNear = nearD; }
							}
						}
					}
				}

				if (mostTop > obMostTop) { mostTop = obMostTop; }
				if (mostBottom < obMostBottom) { mostBottom = obMostBottom; }
				if (mostRight > obMostRight) { mostRight = obMostRight; }
				if (mostLeft < obMostLeft) { mostLeft = obMostLeft; }
				if (mostNear < obMostNear) { mostNear = obMostNear; }
			}

			Vector3 deltaPos((mostRight + mostLeft) * 0.5f, (mostTop + mostBottom) * 0.5f, mostNear);

			mostRight -= deltaPos.getX();
			mostLeft -= deltaPos.getX();
			mostTop -= deltaPos.getY();
			mostBottom -= deltaPos.getY();

			Vector3 newPos;
			newPos = xAxis * deltaPos.getX();
			newPos = newPos + yAxis * deltaPos.getY();
			newPos = newPos + zAxis * deltaPos.getZ();
			view.setPos(Vector3(xAxis | newPos * -1.0f, yAxis | newPos * -1.0f, zAxis | newPos * -1.0f));

			MathOps::CreateOrthoProjEx(proj, mostBottom, mostTop, mostLeft, mostRight, 0.0f, mostNear - mostFar);
		}
		else if (lightData->Type == LightType::SPOT_LIGHT)
		{
			MathOps::LookAt(view, lightData->getPosition(), lightData->getPosition() + lightData->getDirection(), Vector3(0.0f, 1.0f, 0.0f));
			MathOps::CreatePerspectiveProjEx(proj, 1.0f, 2.0 * RadToDeg(acos(lightData->OuterCutOff)), 0.1f, 10.0f);
		}

		lightData->setViewProjMatrix(view * proj);

		drawList->m_shaderFrameData.setViewMat(view);
		drawList->m_shaderFrameData.setProjectionMat(proj);
		drawList->m_mainConstantBuffer->refresh();

		m_currentLight = lightData;
	}

	void ShadowDepthRenderer::setupShadowMapData(LightShadowMapData* shadowMapData)
	{
		m_shaderChain = shadowMapData->normalShaderChain;
		m_skinnedShaderChain = shadowMapData->skinnedShaderChain;
		m_currentFrameBuffer = shadowMapData->dynamicShadowFrameBuffer;

		UInt32 shadowTextureIndex = 0;
		if (shadowMapData->staticShadowTexture)
		{
			m_currentLight->shadowMapIndices[shadowTextureIndex++] = m_shadowTextures.size();
			m_shadowTextures.push_back(shadowMapData->staticShadowTexture);
		}

		if (shadowMapData->dynamicShadowTexture)
		{
			m_currentLight->shadowMapIndices[shadowTextureIndex++] = m_shadowTextures.size();
			m_shadowTextures.push_back(shadowMapData->dynamicShadowTexture);
		}

		while (shadowTextureIndex < 4)
		{
			m_currentLight->shadowMapIndices[shadowTextureIndex++] = -1;
		}
	}

	void ShadowDepthRenderer::reset()
	{
		m_shadowTextures.clear();
	}

	void ShadowDepthRenderer::beginRender()
	{
		if (!m_currentFrameBuffer) { return; }

		m_currentFrameBuffer->bind();
		gGameContext->getRenderer()->Clear(EClearBit::DEPTH_BUFFER_BIT);

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
		gGameContext->getDrawList()->m_mainConstantBuffer->bindAndRefresh();
		shaderChain->bindConstantBuffer("frame_data", gGameContext->getDrawList()->m_mainConstantBuffer);

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
		getInstance()->setupLightData(lightData, drawList);
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
		for (UInt32 i = 0; i < renderer->m_shadowTextures.size(); i++)
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
		}

		return s_shadowRenderer;
	}

	ZE::ShadowDepthRenderer* ShadowDepthRenderer::s_shadowRenderer = nullptr;

}