#include "PhysXBody.h"

#include "Utils/Array.h"

namespace ZE
{

	PhysXBody::PhysXBody(physx::PxRigidActor* _rigidActor) 
		: m_physXRigidActor(_rigidActor)
	{
		m_physXRigidActor->userData = this;
	}

	void PhysXBody::setupPhysicsBody()
	{
		m_filterData.word0 = m_collisionGroup;
		if (m_bCollisionEnabled)
		{
			m_filterData.word1 = m_collisionGroupsMask;
			m_filterData.word2 = m_bGenerateTouchEvent ? m_collisionGroupsMask : 0;
		}
		else
		{
			m_filterData.word1 = 0;
			m_filterData.word2 = 0;
		}

		m_physXRigidActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, m_bEnableGravity ? false : true);

		const physx::PxU32 numShapes = m_physXRigidActor->getNbShapes();
		Array<physx::PxShape*> shapes(numShapes);
		m_physXRigidActor->getShapes(&shapes[0], numShapes);
		for (UInt32 i = 0; i < numShapes; i++)
		{
			physx::PxShape* shape = shapes[i];
			shape->setSimulationFilterData(m_filterData);
			shape->setQueryFilterData(m_filterData);

			if (m_bTriggerOnly)
			{
				shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
				shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, true);
			}
			else
			{
				shape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
				shape->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, false);
			}
		}
	}

	void PhysXBody::setMass(Float32 _mass)
	{
		IPhysicsBody::setMass(_mass);
		if (m_physXRigidActor->is<physx::PxRigidDynamic>())
		{
			physx::PxRigidDynamic* dyn = (physx::PxRigidDynamic*)(m_physXRigidActor);
			dyn->setMass(m_mass);
		}
	}

	ZE::Float32 PhysXBody::getMass() const
	{
		if (m_physXRigidActor->is<physx::PxRigidDynamic>())
		{
			physx::PxRigidDynamic* dyn = (physx::PxRigidDynamic*)(m_physXRigidActor);
			return dyn->getMass();
		}

		return IPhysicsBody::getMass();
	}

	void PhysXBody::AddForceAtPos(const Vector3& globalPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse /*= false*/)
	{
		if (m_physXRigidActor->is<physx::PxRigidBody>())
		{
			physx::PxRigidBody* rigidBody = (physx::PxRigidBody*)(m_physXRigidActor);
			physx::PxVec3 pxGlobalPos(globalPos.getX(), globalPos.getY(), globalPos.getZ());
			physx::PxVec3 pxForce(forceDir.getX() * forceValue, forceDir.getY() * forceValue, forceDir.getZ() * forceValue);
			physx::PxForceMode::Enum mode = (bImpulse) ? physx::PxForceMode::eIMPULSE : physx::PxForceMode::eFORCE;
			physx::PxRigidBodyExt::addForceAtPos(*rigidBody, pxForce, pxGlobalPos, mode);
		}
	}

	void PhysXBody::AddForceAtLocalPos(const Vector3& localPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse /*= false*/)
	{
		if (m_physXRigidActor->is<physx::PxRigidBody>())
		{
			physx::PxRigidBody* rigidBody = (physx::PxRigidBody*)(m_physXRigidActor);
			physx::PxVec3 pxLocalPos(localPos.getX(), localPos.getY(), localPos.getZ());
			physx::PxVec3 pxForce(forceDir.getX() * forceValue, forceDir.getY() * forceValue, forceDir.getZ() * forceValue);
			physx::PxForceMode::Enum mode = (bImpulse) ? physx::PxForceMode::eIMPULSE : physx::PxForceMode::eFORCE;
			physx::PxRigidBodyExt::addForceAtLocalPos(*rigidBody, pxForce, pxLocalPos, mode);
		}
	}

}