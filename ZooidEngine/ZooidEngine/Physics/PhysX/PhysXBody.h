#ifndef __PHYSX_BODY_H__
#define __PHYSX_BODY_H__

#include "Physics/PhysicsBody.h"

#include "PxPhysicsAPI.h"

namespace ZE
{

	class PhysXBody : public IPhysicsBody
	{

	public:
		PhysXBody(physx::PxRigidActor* _rigidActor);

		physx::PxRigidActor* getRigidActor() const { return m_physXRigidActor; }

		virtual void setupCollision() override;

	private:
		physx::PxRigidActor* m_physXRigidActor;
		physx::PxFilterData m_filterData;
	};
}

#endif
