#include "PhysX.h"
#include "PhysXBody.h"
#include "Memory/MemoryHelper.h"
#include "GameObjectModel/Component.h"
#include "Physics/PhysicsEvents.h"
#include "Renderer/DebugRenderer.h"

#include "PxPhysicsAPI.h"

#define PVD_HOST "127.0.0.1"

using namespace physx;

namespace ZE
{

	class DefaultQueryFilterCallback : public PxQueryFilterCallback
	{
	public:
		DefaultQueryFilterCallback(Array<Component*, true>& ignoredComponent)
			: m_ignoredComponent(ignoredComponent)
		{}

		virtual PxQueryHitType::Enum preFilter(const PxFilterData& filterData, const PxShape* shape, const PxRigidActor* actor, PxHitFlags& queryFlags)
		{
			// Check trigger
			if (shape->getFlags() & PxShapeFlag::eTRIGGER_SHAPE)
			{
				return PxQueryHitType::eNONE;
			}
			else if(m_ignoredComponent.size() > 0)
			{
				IPhysicsBody* pPhysicsBody = reinterpret_cast<IPhysicsBody*>(actor->userData);
				for (int i = 0; i < m_ignoredComponent.size(); i++)
				{
					if (pPhysicsBody->getGameObject() == m_ignoredComponent[i])
					{
						return PxQueryHitType::eNONE;
					}
				}
			}

			return PxQueryHitType::eBLOCK;
		}

		virtual PxQueryHitType::Enum postFilter(const PxFilterData& filterData, const PxQueryHit& hit)
		{
			return PxQueryHitType::eBLOCK;
		}

	protected:
		Array<Component*, true> m_ignoredComponent;
	};

	PxDefaultAllocator mainDefaultAllocator;
	PxDefaultErrorCallback mainDefaultErrorCallback;

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
		sceneDesc.flags = physx::PxSceneFlag::eENABLE_ACTIVE_ACTORS | physx::PxSceneFlag::eEXCLUDE_KINEMATICS_FROM_ACTIVE_ACTORS;
		sceneDesc.simulationEventCallback = this;
		m_physxScene = m_physxPhysics->createScene(sceneDesc);

		physx::PxPvdSceneClient* pvdClient = m_physxScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}

		m_defaultPhysicsMaterial = m_physxPhysics->createMaterial(0.5f, 0.5f, 0.6f);

		// Debug Visualization Setup
		m_physxScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
		m_physxScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
		m_physxScene->setVisualizationParameter(PxVisualizationParameter::eACTOR_AXES, 1.0f);

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

			for (UInt32 i = 0; i < nbActors; i++)
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
		const PxRenderBuffer& rb = m_physxScene->getRenderBuffer();
		for (UInt32 i = 0; i < rb.getNbLines(); ++i)
		{
			const PxDebugLine& line = rb.getLines()[i];
			// Extract color UInt ARGB
			UInt32 red = (line.color0 & 0x00ff0000) >> 16;
			UInt32 green = (line.color0 & 0x0000ff00) >> 8;
			UInt32 blue = (line.color0 & 0x000000ff);

			Vector3 color;
			color.setX(red / 255.0f);
			color.setY(green / 255.0f);
			color.setZ(blue / 255.0f);

			DebugRenderer::GetInstance()->drawLine(Vector3(line.pos0.x, line.pos0.y, line.pos0.z),
				Vector3(line.pos1.x, line.pos1.y, line.pos1.z),
				color);
		}
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

		body->setMass(_setup->Mass);
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

	bool PhysXEngine::DoLineRaycast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxRaycastBuffer pxHit;
		PxVec3 pxStartPos(startPos.getX(), startPos.getY(), startPos.getZ());
		PxVec3 pxUnitDir(dir.getX(), dir.getY(), dir.getZ());

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::eANY_HIT;
		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->raycast(pxStartPos, pxUnitDir, distance, pxHit, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);

		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&pxHit.block.position[0]);
			hit.BlockedNormal = Vector3(&pxHit.block.normal[0]);
			if (pxHit.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(pxHit.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		return blocked;
	}

	bool PhysXEngine::DoLineRaycastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxVec3 pxStartPos(startPos.getX(), startPos.getY(), startPos.getZ());
		PxVec3 pxUnitDir(dir.getX(), dir.getY(), dir.getZ());

		const UInt32 bufferSize = 256;
		PxRaycastHit hitBuffer[bufferSize];
		PxRaycastBuffer buff(hitBuffer, bufferSize);

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		queryFilterData.flags |= PxQueryFlag::eNO_BLOCK;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->raycast(pxStartPos, pxUnitDir, distance, buff, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);
		
		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&buff.block.position[0]);
			hit.BlockedNormal = Vector3(&buff.block.normal[0]);
			if (buff.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		hit.bHasTouches = buff.nbTouches > 0;
		for (UInt32 i = 0; i < buff.nbTouches; i++)
		{
			PhysicsHit touchHit;
			touchHit.BlockedPosition = Vector3(&buff.touches[i].position[0]);
			touchHit.BlockedNormal = Vector3(&buff.touches[i].normal[0]);
			if (buff.touches[i].actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.touches[i].actor->userData);
				if (physicsBody->getGameObject())
				{
					touchHit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}

			hit.Touches.push_back(touchHit);
		}

		return blocked;
	}

	bool PhysXEngine::DoBoxCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxSweepBuffer pxHit;
		PxBoxGeometry boxGeom(halfExtent.getX(), halfExtent.getY(), halfExtent.getZ());
		PxTransform initialPose(PxVec3(startPos.getX(), startPos.getY(), startPos.getZ()), PxQuat(quat.getX(), quat.getY(), quat.getZ(), quat.getW()));
		PxVec3 pxDir(dir.getX(), dir.getY(), dir.getZ());

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::eANY_HIT;
		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->sweep(boxGeom, initialPose, pxDir, distance, pxHit, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);

		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&pxHit.block.position[0]);
			hit.BlockedNormal = Vector3(&pxHit.block.normal[0]);
			if (pxHit.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(pxHit.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		return blocked;
	}

	bool PhysXEngine::DoBoxCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxBoxGeometry boxGeom(halfExtent.getX(), halfExtent.getY(), halfExtent.getZ());
		PxTransform initialPose(PxVec3(startPos.getX(), startPos.getY(), startPos.getZ()), PxQuat(quat.getX(), quat.getY(), quat.getZ(), quat.getW()));
		PxVec3 pxDir(dir.getX(), dir.getY(), dir.getZ());

		const UInt32 bufferSize = 256;
		PxSweepHit hitBuffer[bufferSize];
		PxSweepBuffer buff(hitBuffer, bufferSize);

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::eNO_BLOCK;
		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->sweep(boxGeom, initialPose, pxDir, distance, buff, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);

		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&buff.block.position[0]);
			hit.BlockedNormal = Vector3(&buff.block.normal[0]);
			if (buff.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		hit.bHasTouches = buff.nbTouches > 0;
		for (UInt32 i = 0; i < buff.nbTouches; i++)
		{
			PhysicsHit touchHit;
			touchHit.BlockedPosition = Vector3(&buff.touches[i].position[0]);
			touchHit.BlockedNormal = Vector3(&buff.touches[i].normal[0]);
			if (buff.touches[i].actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.touches[i].actor->userData);
				if (physicsBody->getGameObject())
				{
					touchHit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}

			hit.Touches.push_back(touchHit);
		}

		return blocked;
	}

	bool PhysXEngine::DoSphereCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxSweepBuffer pxHit;
		PxSphereGeometry sphereGeom(radius);
		PxTransform initialPose(PxVec3(startPos.getX(), startPos.getY(), startPos.getZ()));
		PxVec3 pxDir(dir.getX(), dir.getY(), dir.getZ());

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::eANY_HIT;
		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->sweep(sphereGeom, initialPose, pxDir, distance, pxHit, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);

		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&pxHit.block.position[0]);
			hit.BlockedNormal = Vector3(&pxHit.block.normal[0]);
			if (pxHit.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(pxHit.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		return blocked;
	}

	bool PhysXEngine::DoSphereCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxSphereGeometry sphereGeom(radius);
		PxTransform initialPose(PxVec3(startPos.getX(), startPos.getY(), startPos.getZ()));
		PxVec3 pxDir(dir.getX(), dir.getY(), dir.getZ());

		const UInt32 bufferSize = 256;
		PxSweepHit hitBuffer[bufferSize];
		PxSweepBuffer buff(hitBuffer, bufferSize);

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::eNO_BLOCK;
		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->sweep(sphereGeom, initialPose, pxDir, distance, buff, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);

		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&buff.block.position[0]);
			hit.BlockedNormal = Vector3(&buff.block.normal[0]);
			if (buff.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		hit.bHasTouches = buff.nbTouches > 0;
		for (UInt32 i = 0; i < buff.nbTouches; i++)
		{
			PhysicsHit touchHit;
			touchHit.BlockedPosition = Vector3(&buff.touches[i].position[0]);
			touchHit.BlockedNormal = Vector3(&buff.touches[i].normal[0]);
			if (buff.touches[i].actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.touches[i].actor->userData);
				if (physicsBody->getGameObject())
				{
					touchHit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}

			hit.Touches.push_back(touchHit);
		}

		return blocked;
	}

	bool PhysXEngine::DoCapsuleCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxSweepBuffer pxHit;
		PxCapsuleGeometry sphereGeom(radius, halfHeight);
		PxTransform initialPose(PxVec3(startPos.getX(), startPos.getY(), startPos.getZ()), PxQuat(quat.getX(), quat.getY(), quat.getZ(), quat.getW()));
		PxVec3 pxDir(dir.getX(), dir.getY(), dir.getZ());

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::eANY_HIT;
		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->sweep(sphereGeom, initialPose, pxDir, distance, pxHit, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);

		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&pxHit.block.position[0]);
			hit.BlockedNormal = Vector3(&pxHit.block.normal[0]);
			if (pxHit.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(pxHit.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		return blocked;
	}

	bool PhysXEngine::DoCapsuleCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoredComponents)
	{
		PxCapsuleGeometry sphereGeom(radius, halfHeight);
		PxTransform initialPose(PxVec3(startPos.getX(), startPos.getY(), startPos.getZ()), PxQuat(quat.getX(), quat.getY(), quat.getZ(), quat.getW()));
		PxVec3 pxDir(dir.getX(), dir.getY(), dir.getZ());

		const UInt32 bufferSize = 256;
		PxSweepHit hitBuffer[bufferSize];
		PxSweepBuffer buff(hitBuffer, bufferSize);

		PxQueryFilterData queryFilterData;
		queryFilterData.data.word0 = _groups;

		queryFilterData.flags |= PxQueryFlag::eNO_BLOCK;
		queryFilterData.flags |= PxQueryFlag::ePREFILTER;
		DefaultQueryFilterCallback filterCallback(ignoredComponents);

		bool blocked = m_physxScene->sweep(sphereGeom, initialPose, pxDir, distance, buff, PxHitFlag::eDEFAULT, queryFilterData, &filterCallback);

		hit.bIsBlocked = blocked;
		if (blocked)
		{
			hit.BlockedPosition = Vector3(&buff.block.position[0]);
			hit.BlockedNormal = Vector3(&buff.block.normal[0]);
			if (buff.block.actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.block.actor->userData);
				if (physicsBody->getGameObject())
				{
					hit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}
		}

		hit.bHasTouches = buff.nbTouches > 0;
		for (UInt32 i = 0; i < buff.nbTouches; i++)
		{
			PhysicsHit touchHit;
			touchHit.BlockedPosition = Vector3(&buff.touches[i].position[0]);
			touchHit.BlockedNormal = Vector3(&buff.touches[i].normal[0]);
			if (buff.touches[i].actor)
			{
				IPhysicsBody* physicsBody = reinterpret_cast<IPhysicsBody*>(buff.touches[i].actor->userData);
				if (physicsBody->getGameObject())
				{
					touchHit.BlockedComponent = static_cast<Component*>(physicsBody->getGameObject());
				}
			}

			hit.Touches.push_back(touchHit);
		}

		return blocked;
	}

	bool PhysXEngine::BoxOverlap(const Vector3& position, const Quaternion& quat, const Vector3& halfExtent, Array<IPhysicsBody*, true>& physicsBodyResult)
	{
		const UInt32 hitSize = 256;
		PxOverlapHit hitBuffer[hitSize];
		PxOverlapBuffer hit(hitBuffer, hitSize);

		PxBoxGeometry boxGeom(PxVec3(halfExtent.getX(), halfExtent.getY(), halfExtent.getZ()));
		PxTransform shapePose(PxVec3(position.getX(), position.getY(), position.getZ()), PxQuat(quat.getX(), quat.getY(), quat.getZ(), quat.getW()));

		bool status = m_physxScene->overlap(boxGeom, shapePose, hit);

		for (UInt32 i = 0; i < hit.nbTouches; i++)
		{
			PxOverlapHit& touchHit = hit.touches[i];
			if (touchHit.actor && touchHit.actor->userData)
			{
				IPhysicsBody* physicBody = reinterpret_cast<IPhysicsBody*>(touchHit.actor->userData);
				physicsBodyResult.push_back(physicBody);
			}
		}

		return status;
	}

	bool PhysXEngine::SphereOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Array<IPhysicsBody*, true>& physicsBodyResult)
	{
		const UInt32 hitSize = 256;
		PxOverlapHit hitBuffer[hitSize];
		PxOverlapBuffer hit(hitBuffer, hitSize);

		PxSphereGeometry sphereGeom(radius);
		PxTransform shapePose(PxVec3(position.getX(), position.getY(), position.getZ()), PxQuat(quat.getX(), quat.getY(), quat.getZ(), quat.getW()));

		bool status = m_physxScene->overlap(sphereGeom, shapePose, hit);

		for (UInt32 i = 0; i < hit.nbTouches; i++)
		{
			PxOverlapHit& touchHit = hit.touches[i];
			if (touchHit.actor && touchHit.actor->userData)
			{
				IPhysicsBody* physicBody = reinterpret_cast<IPhysicsBody*>(touchHit.actor->userData);
				physicsBodyResult.push_back(physicBody);
			}
		}

		return status;
	}

	bool PhysXEngine::CapsuleOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Float32 halfHeight, Array<IPhysicsBody*, true>& physicsBodyResult)
	{
		const UInt32 hitSize = 256;
		PxOverlapHit hitBuffer[hitSize];
		PxOverlapBuffer hit(hitBuffer, hitSize);

		PxCapsuleGeometry capsuleGeom(radius, halfHeight);
		PxTransform shapePose(PxVec3(position.getX(), position.getY(), position.getZ()), PxQuat(quat.getX(), quat.getY(), quat.getZ(), quat.getW()));

		bool status = m_physxScene->overlap(capsuleGeom, shapePose, hit);

		for (UInt32 i = 0; i < hit.nbTouches; i++)
		{
			PxOverlapHit& touchHit = hit.touches[i];
			if (touchHit.actor && touchHit.actor->userData)
			{
				IPhysicsBody* physicBody = reinterpret_cast<IPhysicsBody*>(touchHit.actor->userData);
				physicsBodyResult.push_back(physicBody);
			}
		}

		return status;
	}

	bool PhysXEngine::PhysicsBodyOverlap(IPhysicsBody* physicsBody, Array<IPhysicsBody*, true>& physicsBodyResult)
	{
		PhysXBody* realBody = static_cast<PhysXBody*>(physicsBody);
		PxRigidActor* actor = realBody->getRigidActor();
		
		const physx::PxU32 numShapes = actor->getNbShapes();
		Array<physx::PxShape*> shapes(numShapes);
		actor->getShapes(&shapes[0], numShapes);

		bool status = false;
		for (UInt32 i = 0; i < numShapes; i++)
		{
			PxShape* shape = shapes[i];
			
			const UInt32 hitSize = 256;
			PxOverlapHit hitBuffer[hitSize];
			PxOverlapBuffer hit(hitBuffer, hitSize);

			const PxGeometry& geom = shape->getGeometry().any();
			PxTransform shapePose = PxShapeExt::getGlobalPose(*shape, *actor);

			status = m_physxScene->overlap(geom, shapePose, hit) || status;

			for (UInt32 i = 0; i < hit.nbTouches; i++)
			{
				PxOverlapHit& touchHit = hit.touches[i];
				if (touchHit.actor && touchHit.actor->userData)
				{
					IPhysicsBody* physicBody = reinterpret_cast<IPhysicsBody*>(touchHit.actor->userData);
					if (!physicsBodyResult.contains(physicsBody))
					{
						physicsBodyResult.push_back(physicBody);
					}
				}
			}
		}

		return status;
	}

	void PhysXEngine::onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count)
	{

	}

	void PhysXEngine::onWake(physx::PxActor** actors, physx::PxU32 count)
	{

	}

	void PhysXEngine::onSleep(physx::PxActor** actors, physx::PxU32 count)
	{

	}

	void PhysXEngine::onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs)
	{
		IPhysicsBody* physicBody1 = reinterpret_cast<IPhysicsBody*>(pairHeader.actors[0]->userData);
		IPhysicsBody* physicBody2 = reinterpret_cast<IPhysicsBody*>(pairHeader.actors[1]->userData);
		Component* component = static_cast<Component*>(physicBody1->getGameObject());
		Component* otherComponent = static_cast<Component*>(physicBody2->getGameObject());
		for (UInt32 i = 0; i < nbPairs; i++)
		{
			const physx::PxContactPair& contactPair = pairs[i];

			if (contactPair.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				Event_Physics_ONCOLLIDE collideEvent;
				collideEvent.m_component = component;
				collideEvent.m_otherComponent = otherComponent;

				Array<physx::PxContactPairPoint> contactPairPoints(contactPair.contactCount);
				UInt32 resultCount = contactPair.extractContacts(&contactPairPoints[0], contactPair.contactCount);
				for (UInt32 j = 0; j < resultCount; j++)
				{
					physx::PxContactPairPoint& contactPairPoint = contactPairPoints[j];
					ContactPhysicsData contactPhysicsData;
					contactPhysicsData.MainComponent = component;
					contactPhysicsData.OtherComponent = otherComponent;

					contactPhysicsData.ContactPos = Vector3(contactPairPoint.position.x, contactPairPoint.position.y, contactPairPoint.position.z);
					contactPhysicsData.ContactNormal = Vector3(contactPairPoint.normal.x, contactPairPoint.normal.y, contactPairPoint.normal.z);
					contactPhysicsData.ContactImpulse = Vector3(contactPairPoint.impulse.x, contactPairPoint.impulse.y, contactPairPoint.impulse.z);

					collideEvent.m_contacts.push_back(contactPhysicsData);
				}

				if (component) { component->handleEvent(&collideEvent); }
				if (otherComponent) { otherComponent->handleEvent(&collideEvent); }
			}
		}
	}

	void PhysXEngine::onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count)
	{
		for (UInt32 i = 0; i < count; i++)
		{
			// ignore pairs when shapes have been deleted
			if (pairs[i].flags & (physx::PxTriggerPairFlag::eREMOVED_SHAPE_TRIGGER | physx::PxTriggerPairFlag::eREMOVED_SHAPE_OTHER))
				continue;

			physx::PxTriggerPair& triggerPair = pairs[i];
			if (triggerPair.status == physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
			{
				Event_Physics_ON_BEGIN_TRIGGER beginTrigger;
				IPhysicsBody* triggerPhysicsBody = (IPhysicsBody*)(triggerPair.triggerActor->userData);
				IPhysicsBody* otherPhysicsBody = (IPhysicsBody*)(triggerPair.otherActor->userData);

				beginTrigger.m_triggerComponent = static_cast<Component*>(triggerPhysicsBody->getGameObject());
				beginTrigger.m_otherComponent = static_cast<Component*>(otherPhysicsBody->getGameObject());

				if (beginTrigger.m_triggerComponent) { beginTrigger.m_triggerComponent->handleEvent(&beginTrigger); }
				if (beginTrigger.m_otherComponent) { beginTrigger.m_otherComponent->handleEvent(&beginTrigger); }
			}
			else if (triggerPair.status == physx::PxPairFlag::eNOTIFY_TOUCH_LOST)
			{
				Event_Physics_ON_END_TRIGGER endTrigger;
				IPhysicsBody* triggerPhysicsBody = (IPhysicsBody*)(triggerPair.triggerActor->userData);
				IPhysicsBody* otherPhysicsBody = (IPhysicsBody*)(triggerPair.otherActor->userData);

				endTrigger.m_triggerComponent = static_cast<Component*>(triggerPhysicsBody->getGameObject());
				endTrigger.m_otherComponent = static_cast<Component*>(otherPhysicsBody->getGameObject());

				if (endTrigger.m_triggerComponent) { endTrigger.m_triggerComponent->handleEvent(&endTrigger); }
				if (endTrigger.m_otherComponent) { endTrigger.m_otherComponent->handleEvent(&endTrigger); }
			}
		}
	}

	void PhysXEngine::onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count)
	{

	}

	physx::PxShape* PhysXEngine::CreateShape(PhysicsBodyDesc* _data, Vector3 scale)
	{
		physx::PxShape* pxShape = nullptr;
		physx::PxTransform localTransform(physx::PxVec3(0.0f,0.0f, 0.0f));

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
			localTransform.q = physx::PxQuat(physx::PxHalfPi, physx::PxVec3(0.0f, 0.0f, 1.0f));
			pxShape->setLocalPose(localTransform);
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