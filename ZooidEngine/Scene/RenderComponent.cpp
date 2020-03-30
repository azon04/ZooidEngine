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

#include "Utils/DebugOptions.h"

namespace ZE 
{
	IMPLEMENT_CLASS_1(RenderComponent, SceneComponent)

	void RenderComponent::setupComponent()
	{
		SceneComponent::setupComponent();
		setupPhysics();
		setupMesh();

		addEventDelegate(Event_GATHER_RENDER, &RenderComponent::handleGatherRender);
		addEventDelegate(Event_GATHER_BOUND, &RenderComponent::handleGatherRenderBound);
		addEventDelegate(Event_GATHER_SHADOW_LIST, &RenderComponent::handleGatherShadowRender);
	}

	void RenderComponent::handleGatherRender(Event* pEvent)
	{
		if (m_mesh)
		{
			DrawList* drawList = m_gameContext->getDrawList();

			Sphere boundingSphere = m_mesh->getBoundingSphere(m_worldTransform);
			EFrustumTestResult testResult = m_gameContext->getDrawList()->m_viewFustrum.testSphere(boundingSphere);
			if (testResult == FRUSTUM_OUTSIDE)
			{
				return;
			}
			
			if (testResult == FRUSTUM_INTERSECT)
			{
				testResult = m_gameContext->getDrawList()->m_viewFustrum.testOB(m_mesh->getOBBoundingBox(m_worldTransform));
			}

			if (testResult == FRUSTUM_OUTSIDE)
			{
				return;
			}

			// Draw debug culling
			if (gDebugOptions.DebugDrawOptions.bDrawCullShapes)
			{
				if (testResult == FRUSTUM_INSIDE)
				{
					DebugRenderer::DrawDebugSphere(boundingSphere.m_pos, boundingSphere.m_radius);
				}
				else
				{
					DebugRenderer::DrawDebugBox(m_mesh->getLocalBoxExtend(), m_mesh->getLocalBoxCenter(), m_worldTransform);
				}
			}
			
			if (m_mesh->hasSkeleton())
			{
				SkinMeshRenderInfo* skinMeshRenderInfo = m_gameContext->getDrawList()->m_skinMeshRenderGatherer.nextRenderInfo();
				SceneRenderFactory::InitializeRenderInfoForMesh(skinMeshRenderInfo, m_mesh, m_material);
				if (m_material)
				{
					skinMeshRenderInfo->m_material = m_material;
				}
				skinMeshRenderInfo->m_worldTransform = m_worldTransform;
				skinMeshRenderInfo->m_castShadow = m_bCastShadow;
				skinMeshRenderInfo->m_boxExtent = m_mesh->getLocalBoxExtend();
				skinMeshRenderInfo->m_boxLocalPos = m_mesh->getLocalBoxCenter();

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
				Material* mat = m_material ? m_material : m_mesh->getMaterial();
				if (mat->IsBlend())
				{
					meshRenderInfo = m_gameContext->getDrawList()->m_transculentRenderGatherer.nextRenderInfo();
				}
				else
				{
					meshRenderInfo = m_gameContext->getDrawList()->m_meshRenderGatherer.nextRenderInfo();
				}
				SceneRenderFactory::InitializeRenderInfoForMesh(meshRenderInfo, m_mesh, m_material);
				meshRenderInfo->m_material = mat;
				meshRenderInfo->m_worldTransform = m_worldTransform;
				meshRenderInfo->m_castShadow = m_bCastShadow;
				meshRenderInfo->m_boxExtent = m_mesh->getLocalBoxExtend();
				meshRenderInfo->m_boxLocalPos = m_mesh->getLocalBoxCenter();
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

	void RenderComponent::handleGatherRenderBound(Event* pEvent)
	{
		// Calculate total Bounding Box
		if(m_mesh)
		{
			DrawList* drawList = m_gameContext->getDrawList();

			AxisAlignedBox objectBounding = m_mesh->getAABBoundingBox(m_worldTransform);

			if (drawList->m_objectsBounding.m_min.m_x > objectBounding.m_min.m_x)
			{
				drawList->m_objectsBounding.m_min.m_x = objectBounding.m_min.m_x;
			}

			if (drawList->m_objectsBounding.m_min.m_y > objectBounding.m_min.m_y)
			{
				drawList->m_objectsBounding.m_min.m_y = objectBounding.m_min.m_y;
			}

			if (drawList->m_objectsBounding.m_min.m_z > objectBounding.m_min.m_z)
			{
				drawList->m_objectsBounding.m_min.m_z = objectBounding.m_min.m_z;
			}

			if (drawList->m_objectsBounding.m_max.m_x < objectBounding.m_max.m_x)
			{
				drawList->m_objectsBounding.m_max.m_x = objectBounding.m_max.m_x;
			}

			if (drawList->m_objectsBounding.m_max.m_y < objectBounding.m_max.m_y)
			{
				drawList->m_objectsBounding.m_max.m_y = objectBounding.m_max.m_y;
			}

			if (drawList->m_objectsBounding.m_max.m_z < objectBounding.m_max.m_z)
			{
				drawList->m_objectsBounding.m_max.m_z = objectBounding.m_max.m_z;
			}
		}
	}

	void RenderComponent::handleGatherShadowRender(Event* pEvent)
	{
		if (m_mesh && m_bCastShadow)
		{
			Event_GATHER_SHADOW_LIST* pRealEvent = static_cast<Event_GATHER_SHADOW_LIST*>(pEvent);

			DrawList* drawList = m_gameContext->getDrawList();

			LightShadowMapData& shadowMapData = drawList->m_lightShadowMapData[pRealEvent->m_shadowDataIndex];

			Sphere boundingSphere = m_mesh->getBoundingSphere(m_worldTransform);
			EFrustumTestResult testResult = shadowMapData.lightFrustum.testSphere(boundingSphere);
			if (testResult == FRUSTUM_OUTSIDE)
			{
				return;
			}

			if (m_mesh->hasSkeleton())
			{
				SkinMeshRenderInfo* skinMeshRenderInfo = shadowMapData.skinMeshRenderGatherer.nextRenderInfo();
				SceneRenderFactory::InitializeRenderInfoForMesh(skinMeshRenderInfo, m_mesh, m_material);
				if (m_material)
				{
					skinMeshRenderInfo->m_material = m_material;
				}
				skinMeshRenderInfo->m_worldTransform = m_worldTransform;

				SkeletonState* pSkeletonState = m_hSkeletonState.getObject<SkeletonState>();
				pSkeletonState->updateBuffer();
				skinMeshRenderInfo->m_skinJointData = pSkeletonState->getGPUBufferData();
			}
			else
			{
				MeshRenderInfo* meshRenderInfo = shadowMapData.meshRenderGatherer.nextRenderInfo();
				SceneRenderFactory::InitializeRenderInfoForMesh(meshRenderInfo, m_mesh, m_material);
				if (m_material)
				{
					meshRenderInfo->m_material = m_material;
				}
				meshRenderInfo->m_worldTransform = m_worldTransform;
			}
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

	void RenderComponent::setMaterial(Material* material)
	{
		m_material = material;
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