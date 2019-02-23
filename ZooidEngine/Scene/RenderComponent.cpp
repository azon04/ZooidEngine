#include "RenderComponent.h"

#include "ResourceManagers/MeshManager.h"

#include "Events/Events.h"
#include "Renderer/DrawList.h"
#include "Renderer/IShader.h"
#include "ResourceManagers/ShaderManager.h"
#include "Renderer/DebugRenderer.h"

#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "Physics/PhysicsEvents.h"

#include "Animation/Skeleton.h"
#include "Animation/AnimationComponent.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "ZEGameContext.h"

#include "SceneRenderer/SceneRenderFactory.h"

namespace ZE 
{
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
			if (m_mesh->hasSkeleton())
			{
				SkinMeshRenderInfo* skinMeshRenderInfo = m_gameContext->getDrawList()->m_skinMeshRenderGatherer.nextRenderInfo();
				SceneRenderFactory::InitializeRenderInfoForMesh(skinMeshRenderInfo, m_mesh);
				skinMeshRenderInfo->m_worldTransform = m_worldTransform;
				skinMeshRenderInfo->m_castShadow = m_bCastShadow;

				SkeletonState* pSkeletonState = m_hSkeletonState.getObject<SkeletonState>();
				pSkeletonState->updateBuffer();
				skinMeshRenderInfo->m_skinJointData = pSkeletonState->getGPUBufferData();

#if DEBUG_SKELETON
				for (int i = 0; i < pSkeletonState->getSkeleton()->getJointCount(); i++)
				{
					Matrix4x4 bindPose;
					pSkeletonState->getBindPoseMatrix(i, bindPose);
					DebugRenderer::DrawMatrixBasis(bindPose * m_worldTransform);
				}
#endif
			}
			else
			{
				MeshRenderInfo* meshRenderInfo = nullptr;
				if (m_mesh->getMaterial()->IsBlend())
				{
					meshRenderInfo = m_gameContext->getDrawList()->m_transculentRenderGatherer.nextRenderInfo();
				}
				else
				{
					meshRenderInfo = m_gameContext->getDrawList()->m_meshRenderGatherer.nextRenderInfo();
				}
				SceneRenderFactory::InitializeRenderInfoForMesh(meshRenderInfo, m_mesh);
				meshRenderInfo->m_worldTransform = m_worldTransform;
				meshRenderInfo->m_castShadow = m_bCastShadow;
			}

			/*if (m_bHighlight)
			{
				EnableAndSetStencilFunc(*shaderAction, ALWAYS, 1, 0xFF, 0xFF);

				ShaderAction& highlightAction = m_gameContext->getDrawList()->getNextShaderAction();

				highlightAction.setShaderAndBuffer(ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_3D_HIGHLIGHT), m_mesh->getGPUBufferArray());
				highlightAction.setTopology(TOPOLOGY_TRIANGLE);
				Matrix4x4 m_scaledTransform = m_worldTransform;
				m_scaledTransform.scale(Vector3(1.05f, 1.05f, 1.05f));

				highlightAction.setShaderMatVar("modelMat", m_scaledTransform);
				highlightAction.setShaderVec3Var("highlightColor", Vector3(1.0f, 0.5, 0.3f));

				EnableAndSetStencilFunc(highlightAction, NOTEQUAL, 1, 0xFF, 0x00);
			}*/
		}
	}

	void RenderComponent::loadMeshFromFile(const char* filePath)
	{
		Handle hMesh = MeshManager::GetInstance()->loadResource(filePath);
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
		if (m_mesh && m_mesh->getPhysicsBodySetup().isValid() && m_physicsEnabled)
		{
			PhysicsBodySetup* pPhysicsBodySetup = m_mesh->getPhysicsBodySetup().getObject<PhysicsBodySetup>();
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
			Skeleton* pSkeleton = m_mesh->getSkeletonHandle().getObject<Skeleton>();
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