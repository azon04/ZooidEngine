#include "PhysX.h"
#include "PhysXBody.h"

#include "PxPhysicsAPI.h"

#define PVD_HOST "127.0.0.1"

namespace ZE
{
	physx::PxDefaultAllocator mainDefaultAllocator;
	physx::PxDefaultErrorCallback mainDefaultErrorCallback;

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
		sceneDesc.filterShader = physx::PxDefaultSimulationFilterShader;
		m_physxScene = m_physxPhysics->createScene(sceneDesc);

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

	void PhysXEngine::Update()
	{
		m_physxScene->simulate(1.0f / 60.0f);
		m_physxScene->fetchResults(true);
	}

	void PhysXEngine::PostUpdate()
	{

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

	ZE::Handle PhysXEngine::CreateDynamicRigidBody(PhysicsShape _shape, PhysicsBodyDesc* _data)
	{
		Matrix4x4 transposeMat = _data->Transform.transpose();
		physx::PxMat44 pxMat44(transposeMat.m_data[0][0]);
		physx::PxTransform pxTransform(pxMat44);
		physx::PxRigidDynamic* body = m_physxPhysics->createRigidDynamic(pxTransform);
		if (_shape != COMPOUND_SHAPE)
		{
			physx::PxShape* shape = CreateShape(_shape, _data);
			if (shape) { body->attachShape(*shape); }
		}
		else
		{
			// #TODO attach all compound list
		}

		m_physxScene->addActor(*body);

		Handle hPhysicsBody("Physics Body", sizeof(PhysXBody));
		PhysXBody* pPhysicsBody = new(hPhysicsBody) PhysXBody(body);

		return hPhysicsBody;
	}

	ZE::Handle PhysXEngine::CreateStaticRigidBody(PhysicsShape _shape, PhysicsBodyDesc* _data)
	{
		Matrix4x4 transposeMat = _data->Transform.transpose();
		physx::PxMat44 pxMat44(transposeMat.m_data[0][0]);
		physx::PxTransform pxTransform(pxMat44);
		physx::PxRigidStatic* body = m_physxPhysics->createRigidStatic(pxTransform);
		if (_shape != COMPOUND_SHAPE)
		{
			physx::PxShape* shape = CreateShape(_shape, _data);
			if (shape) { body->attachShape(*shape); }
		}
		else
		{
			// #TODO attach all compound list
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
			physx::PxRigidActor* actor = pPhysicsBody->GetRigidActor();
			m_physxScene->removeActor(*actor);
			actor->release();
		}
	}

	physx::PxShape* PhysXEngine::CreateShape(PhysicsShape _shape, PhysicsBodyDesc* _data)
	{
		physx::PxShape* pxShape = nullptr;
		switch (_shape)
		{
		case ZE::BOX:
		{
			BoxBodyDesc* boxDesc = static_cast<BoxBodyDesc*>(_data);
			pxShape = m_physxPhysics->createShape(
				physx::PxBoxGeometry(boxDesc->HalfExtent.getX(), boxDesc->HalfExtent.getY(), boxDesc->HalfExtent.getZ()),
				*m_defaultPhysicsMaterial);
			break;
		}
		case ZE::SPHERE:
		{
			SphereBodyDesc* sphereDesc = static_cast<SphereBodyDesc*>(_data);
			pxShape = m_physxPhysics->createShape(
				physx::PxSphereGeometry(sphereDesc->Radius),
				*m_defaultPhysicsMaterial);
			break;
		}
		case ZE::CAPSULE:
		{
			CapsuleBodyDesc* capsuleDesc = static_cast<CapsuleBodyDesc*>(_data);
			pxShape = m_physxPhysics->createShape(
				physx::PxCapsuleGeometry(capsuleDesc->Radius, capsuleDesc->HalfHeight),
				*m_defaultPhysicsMaterial);
			break;
		}
		case ZE::PLANE:
		{
			PlaneBodyDesc* planeDesc = static_cast<PlaneBodyDesc*>(_data);
			pxShape = m_physxPhysics->createShape(
				physx::PxPlaneGeometry(),
				*m_defaultPhysicsMaterial);
			break;
		}
		case ZE::CONVEX_MESHES:
		{
			ConvexMeshBodyDesc* convexMeshDesc = static_cast<ConvexMeshBodyDesc*>(_data);
			// #TODO Generate PxConvexMesh based on convexMeshDesc->MeshData
			pxShape = m_physxPhysics->createShape(
				physx::PxConvexMeshGeometry(),
				*m_defaultPhysicsMaterial);
			break;
		}
		case ZE::TRIANGLE_MESHES:
		{
			TriangleMeshBodyDesc* triangleMeshDesc = static_cast<TriangleMeshBodyDesc*> (_data);
			// #TODO Generate PxTriangleMesh based on triangleMeshDesc->TriangleMeshData
			pxShape = m_physxPhysics->createShape(
				physx::PxTriangleMeshGeometry(),
				*m_defaultPhysicsMaterial);
			break;
		}
		case ZE::HEIGHT_FIELDS:
		{
			HeightFieldBodyDesc* heightFieldDesc = static_cast<HeightFieldBodyDesc*>(_data);
			// #TODO Generate PxHeightField based on heightFieldDesc->HeightFieldData
			pxShape = m_physxPhysics->createShape(
				physx::PxHeightFieldGeometry(),
				*m_defaultPhysicsMaterial);
			break;
		}
		case ZE::COMPOUND_SHAPE:
			// #TODO
			break;
		default:
			break;
		}

		if (pxShape && _data->bGhost)
		{
			pxShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
			pxShape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
		}
		return pxShape;
	}

}