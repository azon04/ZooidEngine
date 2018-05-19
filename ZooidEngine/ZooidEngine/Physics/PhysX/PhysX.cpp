#include "PhysX.h"
#include "PhysXBody.h"
#include "Memory/MemoryHelper.h"
#include "GameObjectModel/Component.h"
#include "Physics/PhysicsEvents.h"

#include "PxPhysicsAPI.h"

#define PVD_HOST "127.0.0.1"

namespace ZE
{
	physx::PxDefaultAllocator mainDefaultAllocator;
	physx::PxDefaultErrorCallback mainDefaultErrorCallback;

	physx::PxFilterFlags DefaultGameFilterShader(
		physx::PxFilterObjectAttributes attributes0, physx::PxFilterData filterData0,
		physx::PxFilterObjectAttributes attributes1, physx::PxFilterData filterData1,
		physx::PxPairFlags& pairFlags, const void* contstantBlock, physx::PxU32 constantBlockSize)
	{
		// word0 - Owner Collision Group
		// word1 - Collision Group Mask
		// word2 - Collision Group mask for generating touch event

		// let triggers through
		if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
		{
			if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
			{
				pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
				return physx::PxFilterFlag::eDEFAULT;
			}
			else
			{
				return physx::PxFilterFlag::eKILL;
			}
		}

		if ((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			// generate contacts for all that were not filtered above
			pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;

			// trigger the contact callback for pair (A,B) where
			// the filtermask of A contains the ID of B and vice versa
			if ((filterData0.word0 & filterData1.word2) && (filterData1.word0 & filterData0.word2))
				pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND;

			return physx::PxFilterFlag::eDEFAULT;
		}
		else
		{
			return physx::PxFilterFlag::eKILL;
		}
	}

	void PhysXEngine::Setup()
	{
		
		m_physxFoundation = PxCreateFoundation(PX_FOUNDATION_VERSION, mainDefaultAllocator, mainDefaultErrorCallback);

		ZASSERT(m_physxFoundation != nullptr, "Failed to create PhysXFoundation!");

		bool recordMemoryAllocations = true;

		m_physxPVD = physx::PxCreatePvd(*m_physxFoundation);
		physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);
		m_physxPVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);

		m_physxPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *m_physxFoundation, physx::PxTolerancesScale(), recordMemoryAllocations, m_physxPVD);

		ZASSERT(m_physxPhysics != nullptr, "Failed to create PhysX Physics!");

		physx::PxSceneDesc sceneDesc(m_physxPhysics->getTolerancesScale());
		sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
		m_physxCPUDispatcher = physx::PxDefaultCpuDispatcherCreate(2);
		sceneDesc.cpuDispatcher = m_physxCPUDispatcher;
		sceneDesc.filterShader = ZE::DefaultGameFilterShader;
		m_physxScene = m_physxPhysics->createScene(sceneDesc);

		m_physxScene->setFlag(physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS, true);
		m_physxScene->setFlag(physx::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS, true);

		physx::PxPvdSceneClient* pvdClient = m_physxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		m_defaultPhysicsMaterial = m_physxPhysics->createMaterial(0.5f, 0.5f, 0.6f);
	}

	void PhysXEngine::PreUpdate()
	{

	}

	void PhysXEngine::Update(float _deltaMS)
	{
		// Pick the smallest one, to make sure the simulation is stable
		float elapsedTime = _deltaMS / 1000.0f > 1.0f / 60.0f ? 1.0f / 60.0f : _deltaMS / 1000.0f;
		m_physxScene->simulate(elapsedTime);
		m_physxScene->fetchResults(true);
	}

	Matrix4x4 ConvertToMatrix4x4(physx::PxTransform& _transform)
	{
		physx::PxMat44 mat(_transform);
		Matrix4x4 resultMat;

		resultMat.setU(Vector3(mat.column0.x, mat.column0.y, mat.column0.z));
		resultMat.setV(Vector3(mat.column1.x, mat.column1.y, mat.column1.z));
		resultMat.setN(Vector3(mat.column2.x, mat.column2.y, mat.column2.z));
		resultMat.setPos(Vector3(mat.column3.x, mat.column3.y, mat.column3.z));

		return resultMat;
	}

	void PhysXEngine::PostUpdate()
	{
		physx::PxU32 nbActors;
		physx::PxActor** actors = m_physxScene->getActiveActors(nbActors);

		if (nbActors)
		{	
			Event_Physics_POSTUPDATE eventPostUpdate;
			Event_Physics_UPDATE_TRANSFORM eventUpdateTransform;

			for (int i = 0; i < nbActors; i++)
			{
				physx::PxRigidActor* actor = (physx::PxRigidActor*) actors[i];
				if (!actor->userData)
				{
					continue;
				}
				
				IPhysicsBody* pPhysicsBody = static_cast<IPhysicsBody*>(actor->userData);
				Component* pComponent = (Component*)(pPhysicsBody->getGameObject());
				if (pComponent)
				{
					eventUpdateTransform.m_worldTransform = ConvertToMatrix4x4(actor->getGlobalPose());
					pComponent->handleEvent(&eventPostUpdate);
					pComponent->handleEvent(&eventUpdateTransform);
				}

			}
		}
	}

	void PhysXEngine::DrawDebug()
	{

	}

	void PhysXEngine::Destroy()
	{
		m_physxScene->release();
		m_physxCPUDispatcher->release();
		m_physxPhysics->release();
		
		physx::PxPvdTransport* transport = m_physxPVD->getTransport();
		m_physxPVD->release();
		transport->release();

		m_physxFoundation->release();
	}

	physx::PxTransform ConvertToPxTransform(Matrix4x4& _transform)
	{
		Matrix4x4 transposeMat = _transform;
		transposeMat.normalizeScale();
		float* matrixData;
		matrixData = &transposeMat.m_data[0][0];
		physx::PxMat44 pxMat44(matrixData);
		return physx::PxTransform(pxMat44);
	}

	ZE::Handle PhysXEngine::CreateDynamicRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup)
	{
		physx::PxTransform pxTransform= ConvertToPxTransform(_transform);
		physx::PxRigidDynamic* body = m_physxPhysics->createRigidDynamic(pxTransform);
		for (int i = 0; i < _setup->m_bodies.length(); i++)
		{
			PhysicsBodyDesc& bodyDesc = _setup->m_bodies[i];
			physx::PxShape* shape = CreateShape(&bodyDesc, _transform.extractScale());
			if (shape) { body->attachShape(*shape); }
		}

		m_physxScene->addActor(*body);

		Handle hPhysicsBody("Physics Body", sizeof(PhysXBody));
		PhysXBody* pPhysicsBody = new(hPhysicsBody) PhysXBody(body);

		return hPhysicsBody;
	}

	ZE::Handle PhysXEngine::CreateStaticRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup)
	{
		physx::PxTransform pxTransform = ConvertToPxTransform(_transform);
		physx::PxRigidStatic* body = m_physxPhysics->createRigidStatic(pxTransform);
		for (int i = 0; i < _setup->m_bodies.length(); i++)
		{
			PhysicsBodyDesc& bodyDesc = _setup->m_bodies[i];
			physx::PxShape* shape = CreateShape(&bodyDesc, _transform.extractScale());
			if (shape) { body->attachShape(*shape); }
		}

		m_physxScene->addActor(*body);

		Handle hPhysicsBody("Physics Body", sizeof(PhysXBody));
		PhysXBody* pPhysicsBody = new(hPhysicsBody) PhysXBody(body);

		return hPhysicsBody;
	}

	void PhysXEngine::DestroyPhysicsObject(Handle handle)
	{
		PhysXBody* pPhysicsBody = handle.getObject<PhysXBody>();
		if (pPhysicsBody)
		{
			physx::PxRigidActor* actor = pPhysicsBody->getRigidActor();
			m_physxScene->removeActor(*actor);
			actor->release();
		}
	}

	physx::PxShape* PhysXEngine::CreateShape(PhysicsBodyDesc* _data, Vector3 scale)
	{
		physx::PxShape* pxShape = nullptr;
		switch (_data->ShapeType)
		{
		case ZE::BOX:
		{
			pxShape = m_physxPhysics->createShape(
				physx::PxBoxGeometry(_data->HalfExtent.getX() * scale.getX(), _data->HalfExtent.getY() * scale.getY(), _data->HalfExtent.getZ() * scale.getZ()),
				*m_defaultPhysicsMaterial, true);
			break;
		}
		case ZE::SPHERE:
		{
			pxShape = m_physxPhysics->createShape(
				physx::PxSphereGeometry(_data->Radius * scale.getX()),
				*m_defaultPhysicsMaterial, true);
			break;
		}
		case ZE::CAPSULE:
		{
			pxShape = m_physxPhysics->createShape(
				physx::PxCapsuleGeometry(_data->Radius * scale.getX(), _data->HalfHeight * scale.getY()),
				*m_defaultPhysicsMaterial, true);
			break;
		}
		case ZE::PLANE:
		{
			pxShape = m_physxPhysics->createShape(
				physx::PxPlaneGeometry(),
				*m_defaultPhysicsMaterial, true);
			break;
		}
		case ZE::CONVEX_MESHES:
		{
			// #TODO Generate PxConvexMesh based on convexMeshDesc->MeshData
			pxShape = m_physxPhysics->createShape(
				physx::PxConvexMeshGeometry(),
				*m_defaultPhysicsMaterial, true);
			break;
		}
		case ZE::TRIANGLE_MESHES:
		{
			// #TODO Generate PxTriangleMesh based on triangleMeshDesc->TriangleMeshData
			pxShape = m_physxPhysics->createShape(
				physx::PxTriangleMeshGeometry(),
				*m_defaultPhysicsMaterial, true);
			break;
		}
		case ZE::HEIGHT_FIELDS:
		{
			// #TODO Generate PxHeightField based on heightFieldDesc->HeightFieldData
			pxShape = m_physxPhysics->createShape(
				physx::PxHeightFieldGeometry(),
				*m_defaultPhysicsMaterial, true);
			break;
		}
		default:
			break;
		}

		return pxShape;
	}

}