#include "PhysXBody.h"

#include "Utils/Array.h"

namespace ZE
{

	PhysXBody::PhysXBody(physx::PxRigidActor* _rigidActor) 
		: m_physXRigidActor(_rigidActor)
	{
		m_physXRigidActor->userData = this;
	}

	void PhysXBody::setupCollision()
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

		const physx::PxU32 numShapes = m_physXRigidActor->getNbShapes();
		Array<physx::PxShape*> shapes(numShapes);
		m_physXRigidActor->getShapes(&shapes[0], numShapes);
		for (int i = 0; i < numShapes; i++)
		{
			physx::PxShape* shape = shapes[i];
			shape->setSimulationFilterData(m_filterData);
		}
	}

}