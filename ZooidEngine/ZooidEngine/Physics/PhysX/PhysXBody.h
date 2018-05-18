#ifndef __PHYSX_BODY_H__
#define __PHYSX_BODY_H__

#include "Physics/PhysicsBody.h"

namespace physx
{
	class PxRigidActor;
}

namespace ZE
{

	class PhysXBody : public IPhysicsBody
	{

	public:
		PhysXBody(physx::PxRigidActor* _rigidActor);

		physx::PxRigidActor* getRigidActor() const { return m_physXRigidActor; }

	private:
		physx::PxRigidActor* m_physXRigidActor;

	};
}

#endif
