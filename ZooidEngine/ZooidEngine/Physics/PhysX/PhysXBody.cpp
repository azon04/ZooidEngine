#include "PhysXBody.h"

#include "PxPhysicsAPI.h"

namespace ZE
{

	PhysXBody::PhysXBody(physx::PxRigidActor* _rigidActor) 
		: m_physXRigidActor(_rigidActor)
	{
		m_physXRigidActor->userData = this;
	}

}