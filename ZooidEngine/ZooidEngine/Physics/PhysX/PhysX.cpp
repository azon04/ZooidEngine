#include "PhysX.h"

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
	}

	void PhysXEngine::PreUpdate()
	{

	}

	void PhysXEngine::Update()
	{

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

	ZE::Handle PhysXEngine::CreateTriggerCollision(PhysicsShape _shape, void* _data)
	{
		return Handle();
	}

	ZE::Handle PhysXEngine::CreateDynamicRigidBody(PhysicsShape _shape, void* _data)
	{
		return Handle();
	}

	ZE::Handle PhysXEngine::CreateStaticRigidBody(PhysicsShape _shape, void* _data)
	{
		return Handle();
	}

	void PhysXEngine::DestroyPhysicsObject(Handle handle)
	{

	}

}