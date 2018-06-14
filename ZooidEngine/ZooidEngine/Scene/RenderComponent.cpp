#include "RenderComponent.h"

#include "ResourceManagers/MeshManager.h"

#include "Events/Events.h"
#include "Renderer/DrawList.h"
#include "Renderer/ShaderAction.h"
#include "Renderer/IShader.h"
#include "ResourceManagers/ShaderManager.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsEvents.h"

#include "Animation/Skeleton.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "ZEGameContext.h"

namespace ZE {
	IMPLEMENT_CLASS_1(RenderComponent, SceneComponent)

	void RenderComponent::setupComponent()
	{
		SceneComponent::setupComponent();
		setupPhysics();
		setupMesh();

		addEventDelegate(Event_GATHER_RENDER, &RenderComponent::handleGatherRender);
	}

	void RenderComponent::handleGatherRender(Event* pEvent)
	{
		if (m_mesh)
		{
			ShaderAction* shaderAction;
			IShaderChain* shader;
			if (m_mesh->m_material && m_mesh->m_material->m_isBlend)
			{
				shaderAction = &m_gameContext->getDrawList()->getNextSecondPassShaderAction();
				shader = ShaderManager::getInstance()
					->getShaderChain(m_mesh->hasSkeleton() ? Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT_BLEND : Z_SHADER_CHAIN_3D_DEFAULT_LIT_BLEND);
				EnableAndSetBlendFunc(*shaderAction, SRC_ALPHA, ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				shaderAction = &m_gameContext->getDrawList()->getNextShaderAction();
				shader = ShaderManager::getInstance()
					->getShaderChain(m_mesh->hasSkeleton() ? Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT : Z_SHADER_CHAIN_3D_DEFAULT_LIT);
			}

			if (m_mesh->m_doubleSided)
			{
				shaderAction->AddShaderFeature(FACE_CULING, false);
			}

			shaderAction->SetShaderAndBuffer(shader, m_mesh->m_bufferArray);
			shaderAction->SetShaderMatVar("modelMat", m_worldTransform);
			
			m_mesh->m_material ? m_mesh->m_material->Bind(*shaderAction) : nullptr;

			shaderAction->SetConstantsBlockBuffer("shader_data", m_gameContext->getDrawList()->m_mainConstantBuffer);
			shaderAction->SetConstantsBlockBuffer("light_data", m_gameContext->getDrawList()->m_lightConstantBuffer);
			
			if (m_mesh->hasSkeleton() && m_hSkeletonState.isValid())
			{
				SkeletonState* pSkeletonState = m_hSkeletonState.getObject<SkeletonState>();
				char buffer[32];
				for (int i = 0; i < pSkeletonState->getSkeleton()->getJointCount(); i++)
				{
					Matrix4x4 jointPallete;
					pSkeletonState->getJointMatrixPallete(i, jointPallete);
					StringFunc::PrintToString(buffer, 32, "boneMats[%d]", i);
					shaderAction->SetShaderMatVar(buffer, jointPallete);
				}
			}

			if (m_bHighlight)
			{
				EnableAndSetStencilFunc(*shaderAction, ALWAYS, 1, 0xFF, 0xFF);

				ShaderAction& highlightAction = m_gameContext->getDrawList()->getNextShaderAction();

				highlightAction.SetShaderAndBuffer(ShaderManager::getInstance()->getShaderChain(Z_SHADER_CHAIN_3D_HIGHLIGHT), m_mesh->m_bufferArray);
				Matrix4x4 m_scaledTransform = m_worldTransform;
				m_scaledTransform.scale(Vector3(1.05f, 1.05f, 1.05f));

				highlightAction.SetShaderMatVar("modelMat", m_scaledTransform);
				highlightAction.SetShaderVec3Var("highlightColor", Vector3(1.0f, 0.5, 0.3f));

				EnableAndSetStencilFunc(highlightAction, NOTEQUAL, 1, 0xFF, 0x00);
			}
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

	void RenderComponent::setTriggerOnly(bool _bTriggerOnly)
	{
		m_bTriggerOnly = _bTriggerOnly;
	}

	void RenderComponent::setStatic(bool _bStatic)
	{
		m_bStatic = _bStatic;
	}

	void RenderComponent::setPhysicsEnabled(bool _bEnabled)
	{
		m_physicsEnabled = _bEnabled;
	}

	void RenderComponent::handlePhysicsUpdateTransform(Event* pEvent)
	{
		Event_Physics_UPDATE_TRANSFORM* pRealEvent = static_cast<Event_Physics_UPDATE_TRANSFORM*>(pEvent);
		Matrix4x4 interMatrix = pRealEvent->m_worldTransform;
		interMatrix.scale(m_worldTransform.extractScale());
		m_worldTransform = interMatrix;
	}

	void RenderComponent::setupPhysics()
	{
		if (m_mesh && m_mesh->m_hPhysicsBodySetup.isValid())
		{
			PhysicsBodySetup* pPhysicsBodySetup = m_mesh->m_hPhysicsBodySetup.getObject<PhysicsBodySetup>();
			if (m_bStatic)
			{
				m_hPhysicsBody = m_gameContext->getPhysics()->CreateStaticRigidBody(m_worldTransform, pPhysicsBodySetup);
			}
			else
			{
				m_hPhysicsBody = m_gameContext->getPhysics()->CreateDynamicRigidBody(m_worldTransform, pPhysicsBodySetup);
				addEventDelegate(Event_Physics_UPDATE_TRANSFORM, &RenderComponent::handlePhysicsUpdateTransform);
			}

			if (m_hPhysicsBody.isValid())
			{
				IPhysicsBody* pPhysicsBody = m_hPhysicsBody.getObject<IPhysicsBody>();
				pPhysicsBody->setGameObject(this);
				pPhysicsBody->setCollisionGroup(m_bStatic ? COLLISION_STATIC : COLLISION_DYNAMIC);
				pPhysicsBody->enableCollisionGroups(COLLISION_STATIC | COLLISION_DYNAMIC);
				pPhysicsBody->setTriggerOnly(m_bTriggerOnly);
				pPhysicsBody->setEnableGravity(m_bEnableGravity);
				pPhysicsBody->setupPhysicsBody();
			}
		}
	}

	void RenderComponent::setupMesh()
	{
		if (m_mesh && m_mesh->hasSkeleton())
		{
			Skeleton* pSkeleton = m_mesh->m_hSkeleton.getObject<Skeleton>();
			m_hSkeletonState = Handle("Skeleton State", sizeof(SkeletonState));
			new(m_hSkeletonState) SkeletonState(pSkeleton);
		}
	}

	bool RenderComponent::hasPhysicsBody()
	{
		return m_hPhysicsBody.isValid();
	}

	ZE::IPhysicsBody* RenderComponent::getPhysicsBody()
	{
		return m_hPhysicsBody.getObject<IPhysicsBody>();
	}

}