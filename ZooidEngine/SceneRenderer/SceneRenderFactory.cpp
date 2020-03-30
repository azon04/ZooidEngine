#include "SceneRenderFactory.h"


#include "Renderer/IGPUBufferArray.h"
#include "Resources/Mesh.h"
#include "Resources/Material.h"
#include "Memory/Handle.h"
#include "SceneRenderer/RenderInfo/MeshRenderInfo.h"
#include "SceneRenderer/RenderInfo/SkinMeshRenderInfo.h"
#include "ResourceManagers/ShaderManager.h"

namespace ZE
{
	ZE::MeshRenderInfo* SceneRenderFactory::CreateRenderInfoForMesh(Mesh* mesh, Material* material)
	{
		Handle renderInfoHandle = Handle("RenderInfo", sizeof(MeshRenderInfo));
		MeshRenderInfo* renderInfo = new(renderInfoHandle) MeshRenderInfo;

		if (!material)
		{
			material = mesh->getMaterial();
		}

		if (material && material->IsBlend())
		{
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(mesh->hasSkeleton() ? Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT_BLEND : Z_SHADER_CHAIN_3D_DEFAULT_LIT_BLEND);
		}
		else
		{
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(mesh->hasSkeleton() ? Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT : Z_SHADER_CHAIN_3D_DEFAULT_LIT);
		}

		renderInfo->m_renderTopology = mesh->getRenderTopology();

		renderInfo->m_gpuBufferArray = mesh->getGPUBufferArray();
		renderInfo->m_material = mesh->getMaterial();
		renderInfo->m_instanceCount = 1;
		renderInfo->m_isDoubleSided = mesh->isDoubleSided();
		renderInfo->m_castShadow = true;

		return renderInfo;
	}

	void SceneRenderFactory::InitializeRenderInfoForMesh(MeshRenderInfo* renderInfo, Mesh* mesh, Material* material)
	{
		if (!material)
		{
			material = mesh->getMaterial();
		}

		if (material && material->IsBlend())
		{
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(mesh->hasSkeleton() ? Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT_BLEND : Z_SHADER_CHAIN_3D_DEFAULT_LIT_BLEND);
		}
		else
		{
			renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(mesh->hasSkeleton() ? Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT : Z_SHADER_CHAIN_3D_DEFAULT_LIT);
		}

		renderInfo->m_renderTopology = mesh->getRenderTopology();

		renderInfo->m_gpuBufferArray = mesh->getGPUBufferArray();
		renderInfo->m_material = mesh->getMaterial();
		renderInfo->m_instanceCount = 1;
		renderInfo->m_isDoubleSided = mesh->isDoubleSided();
		renderInfo->m_castShadow = true;
		renderInfo->drawCount = mesh->getGPUBufferArray()->getDataCount();
	}

	ZE::RenderInfo* SceneRenderFactory::CreateRenderInfoForBufferArray(IGPUBufferArray* bufferArray)
	{
		Handle renderInfoHandle = Handle("RenderInfo", sizeof(RenderInfo));
		RenderInfo* renderInfo = new(renderInfoHandle) MeshRenderInfo;

		// Need to assign to something else after this
		renderInfo->m_shaderChain = nullptr;
		
		renderInfo->m_gpuBufferArray = bufferArray;
		renderInfo->m_renderTopology = bufferArray->getRenderTopology();

		return renderInfo;
	}

}