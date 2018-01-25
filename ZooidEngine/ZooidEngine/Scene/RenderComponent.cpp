#include "RenderComponent.h"

#include "ResourceManagers/MeshManager.h"

#include "Events/Events.h"
#include "Renderer/DrawList.h"
#include "Renderer/ShaderAction.h"
#include "Renderer/Shader.h"
#include "ResourceManagers/ShaderManager.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "ZEGameContext.h"

namespace ZE {
	IMPLEMENT_CLASS_1(RenderComponent, SceneComponent)

	void RenderComponent::setupComponent()
	{
		addEventDelegate(Event_GATHER_RENDER, &RenderComponent::handleGatherRender);
	}

	void RenderComponent::handleGatherRender(Event* pEvent)
	{
		if (m_mesh)
		{
			ShaderAction& shaderAction = m_gameContext->getDrawList()->getNextShaderAction();
			ShaderChain* shader = ShaderManager::getInstance()->getShaderChain(Z_SHADER_CHAIN_3D_DEFAULT_LIT);

			shaderAction.SetShaderAndBuffer(shader, m_mesh->m_bufferArray);
			shaderAction.SetShaderMatVar("modelMat", m_worldTransform);
			
			m_mesh->m_material ? m_mesh->m_material->Bind(shaderAction) : nullptr;

			shaderAction.SetConstantsBlockBuffer("shader_data", m_gameContext->getDrawList()->m_mainConstantBuffer);
			shaderAction.SetConstantsBlockBuffer("light_data", m_gameContext->getDrawList()->m_lightConstantBuffer);
		}
	}

	void RenderComponent::fromFile(const char* filePath)
	{
		Handle hMesh = MeshManager::getInstance()->loadResource(filePath);
		if (hMesh.isValid())
		{
			m_mesh = hMesh.getObject<Mesh>();
		}
	}

}