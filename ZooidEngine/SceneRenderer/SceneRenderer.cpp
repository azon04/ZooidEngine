#include "SceneRenderer.h"

#include "Memory/Handle.h"
#include "Renderer/IShader.h"
#include "RenderInfo/MeshRenderInfo.h"
#include "Renderer/IGPUBufferArray.h"
#include "ZEGameContext.h"
#include "Renderer/IRenderer.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Renderer/DrawList.h"
#include "ShadowRenderer.h"

namespace ZE
{
	void MeshSceneRenderer::beginRender()
	{

	}

	void MeshSceneRenderer::render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		MeshRenderInfo* meshRenderInfos = static_cast<MeshRenderInfo*>(renderInfos);

		for (UInt32 index = 0; index < renderInfoCount; index++)
		{
			MeshRenderInfo& currentMesh = meshRenderInfos[index];

			ZASSERT(currentMesh.m_shaderChain);
			ZASSERT(currentMesh.m_gpuBufferArray);

			currentMesh.m_shaderChain->bind();
			currentMesh.m_shaderChain->setMat("modelMat", currentMesh.m_worldTransform);
			
			// Bind shader_data
			gGameContext->getDrawList()->m_mainConstantBuffer->bindAndRefresh();
			currentMesh.m_shaderChain->bindConstantBuffer("shader_data", gGameContext->getDrawList()->m_mainConstantBuffer);

			// Bind light_data
			gGameContext->getDrawList()->m_lightConstantBuffer->bindAndRefresh();
			currentMesh.m_shaderChain->bindConstantBuffer("light_data", gGameContext->getDrawList()->m_lightConstantBuffer);

			// Set Face Culling for double sided
			if (currentMesh.m_isDoubleSided)
			{
				gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW,
					ECullFace::CULL_NONE,
					ERenderFillMode::MODE_FILL>::GetStatic());
			}
			else
			{
				gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW,
					ECullFace::BACK,
					ERenderFillMode::MODE_FILL>::GetStatic());
			}

			currentMesh.m_gpuBufferArray->bind();

			if (currentMesh.m_material) 
			{
				currentMesh.m_material->Bind(currentMesh.m_shaderChain);
			}

			ShadowDepthRenderer::BindShadowTextures(gGameContext->getDrawList(), currentMesh.m_shaderChain, currentMesh.m_material ? currentMesh.m_material->getTextureCount() : 0);

			gGameContext->getRenderer()->DrawArray(currentMesh.m_renderTopology, 0, currentMesh.drawCount);
		}
	}

	void MeshSceneRenderer::endRender()
	{

	}

	void MeshSceneRenderer::Render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		MeshSceneRenderer* renderer = GetInstance();
		renderer->beginRender();

		renderer->render(renderInfos, renderInfoCount);

		renderer->endRender();
	}

	void TransculentSceneRenderer::beginRender()
	{

	}

	void TransculentSceneRenderer::render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		Array<UInt32> sortIndices;
		MeshRenderInfo* meshInfos = static_cast<MeshRenderInfo*>(renderInfos);
		sortBlendInfos(meshInfos, renderInfoCount, sortIndices);

		for (UInt32 i = 0; i < renderInfoCount; i++)
		{
			UInt32 index = sortIndices[i];
			MeshRenderInfo& currentMesh = meshInfos[index];

			ZASSERT(currentMesh.m_shaderChain);
			ZASSERT(currentMesh.m_gpuBufferArray);

			currentMesh.m_shaderChain->bind();
			currentMesh.m_shaderChain->setMat("modelMat", currentMesh.m_worldTransform);

			// Bind shader_data
			gGameContext->getDrawList()->m_mainConstantBuffer->bindAndRefresh();
			currentMesh.m_shaderChain->bindConstantBuffer("shader_data", gGameContext->getDrawList()->m_mainConstantBuffer);

			// Bind light_data
			gGameContext->getDrawList()->m_lightConstantBuffer->bindAndRefresh();
			currentMesh.m_shaderChain->bindConstantBuffer("light_data", gGameContext->getDrawList()->m_lightConstantBuffer);

			// Set Face Culling for double sided
			if (currentMesh.m_isDoubleSided)
			{
				gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW,
					ECullFace::CULL_NONE,
					ERenderFillMode::MODE_FILL>::GetStatic());
			}
			else
			{
				gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW,
					ECullFace::BACK,
					ERenderFillMode::MODE_FILL>::GetStatic());
			}

			gGameContext->getRenderer()->SetRenderBlendState(TRenderBlendState<true,
				ERendererBlendFactor::SRC_ALPHA,
				ERendererBlendFactor::ONE_MINUS_SRC_ALPHA,
				0>::GetStatic());

			currentMesh.m_gpuBufferArray->bind();

			if (currentMesh.m_material)
			{
				currentMesh.m_material->Bind(currentMesh.m_shaderChain);
			}

			ShadowDepthRenderer::BindShadowTextures(gGameContext->getDrawList(), currentMesh.m_shaderChain, currentMesh.m_material ? currentMesh.m_material->getTextureCount() : 0);

			gGameContext->getRenderer()->DrawArray(currentMesh.m_renderTopology, 0, currentMesh.drawCount);
		}
	}

	void TransculentSceneRenderer::endRender()
	{
		gGameContext->getRenderer()->SetRenderBlendState(TRenderBlendState<false,
			ERendererBlendFactor::SRC_ALPHA,
			ERendererBlendFactor::ONE_MINUS_SRC_ALPHA,
			0>::GetStatic());
	}

	void TransculentSceneRenderer::Render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		TransculentSceneRenderer* renderer = GetInstance();
		renderer->beginRender();
		renderer->render(renderInfos, renderInfoCount);
		renderer->endRender();
	}

	UInt32 quickSortPartition(Float32* squareDist, UInt32* arr, UInt32 lo, UInt32 hi)
	{
		Float32 pivot = squareDist[arr[hi]];
		Int32 i = lo - 1;
		for (Int32 j = lo; j < hi; j++)
		{
			if (squareDist[arr[j]] > pivot)
			{
				i++;
				UInt32 temp = arr[j];
				arr[j] = arr[i];
				arr[i] = temp;
			}
		}

		i++;
		UInt32 temp = arr[hi];
		arr[hi] = arr[i];
		arr[i] = temp;

		return i;
	}

	void quickSort(Float32* squareDist, UInt32* arr, UInt32 lo, UInt32 hi)
	{
		if (lo < hi)
		{
			UInt32 p = quickSortPartition(squareDist, arr, lo, hi);
			if (p > 0)
			{
				quickSort(squareDist, arr, lo, p - 1);
			}

			if (p < hi)
			{
				quickSort(squareDist, arr, p + 1, hi);
			}
		}
	}

	void TransculentSceneRenderer::sortBlendInfos(MeshRenderInfo* renderInfos, UInt32 renderInfoCount, Array<UInt32>& result)
	{
		if (renderInfoCount == 0) { return; }

		Array<Float32> squareDists(renderInfoCount);
		Vector3& cameraPosition = gGameContext->getDrawList()->m_cameraPosition;

		for (UInt32 i = 0; i < renderInfoCount; i++)
		{
			squareDists[i] = (renderInfos[i].m_worldTransform.getPos() - cameraPosition).lengthSquare();
			result.push_back(i);
		}

		quickSort(squareDists.data(), result.data(), 0, renderInfoCount - 1);
	}

	void SkinMeshSceneRenderer::beginRender()
	{

	}

	void SkinMeshSceneRenderer::render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		SkinMeshRenderInfo* meshRenderInfos = static_cast<SkinMeshRenderInfo*>(renderInfos);

		for (UInt32 index = 0; index < renderInfoCount; index++)
		{
			SkinMeshRenderInfo& currentMesh = meshRenderInfos[index];

			ZASSERT(currentMesh.m_shaderChain);
			ZASSERT(currentMesh.m_gpuBufferArray);

			currentMesh.m_shaderChain->bind();
			currentMesh.m_shaderChain->setMat("modelMat", currentMesh.m_worldTransform);

			// Bind shader_data
			gGameContext->getDrawList()->m_mainConstantBuffer->bindAndRefresh();
			currentMesh.m_shaderChain->bindConstantBuffer("shader_data", gGameContext->getDrawList()->m_mainConstantBuffer);

			// Bind light_data
			gGameContext->getDrawList()->m_lightConstantBuffer->bindAndRefresh();
			currentMesh.m_shaderChain->bindConstantBuffer("light_data", gGameContext->getDrawList()->m_lightConstantBuffer);

			// Bind bone_data
			currentMesh.m_skinJointData->bind();
			currentMesh.m_shaderChain->bindConstantBuffer("bone_mats_block", currentMesh.m_skinJointData);

			// Set Face Culling for double sided
			if (currentMesh.m_isDoubleSided)
			{
				gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW,
					ECullFace::CULL_NONE,
					ERenderFillMode::MODE_FILL>::GetStatic());
			}
			else
			{
				gGameContext->getRenderer()->SetRenderRasterizerState(TRenderRasterizerState<EFaceFrontOrder::CCW,
					ECullFace::BACK,
					ERenderFillMode::MODE_FILL>::GetStatic());
			}

			currentMesh.m_gpuBufferArray->bind();

			if (currentMesh.m_material)
			{
				currentMesh.m_material->Bind(currentMesh.m_shaderChain);
			}

			ShadowDepthRenderer::BindShadowTextures(gGameContext->getDrawList(), currentMesh.m_shaderChain, currentMesh.m_material ? currentMesh.m_material->getTextureCount() : 0);

			gGameContext->getRenderer()->DrawArray(currentMesh.m_renderTopology, 0, currentMesh.drawCount);
		}
	}

	void SkinMeshSceneRenderer::endRender()
	{

	}

	void SkinMeshSceneRenderer::Render(RenderInfo* renderInfos, UInt32 renderInfoCount)
	{
		SkinMeshSceneRenderer* renderer = GetInstance();
		renderer->beginRender();

		renderer->render(renderInfos, renderInfoCount);

		renderer->endRender();
	}
}
