#include "CapsuleComponent.h"

#include "Utils/Array.h"
#include "ZEGameContext.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(CapsuleComponent, CollisionComponent)

	void CapsuleComponent::setupPhysics()
	{
		PhysicsBodySetup bodySetup;
		PhysicsBodyDesc bodyDesc;
		bodyDesc.ShapeType = CAPSULE;
		bodyDesc.Radius = m_radius;
		bodyDesc.HalfHeight = m_height * 0.5f;
		bodySetup.m_bodies.push_back(bodyDesc);

		hPhysicsBody = m_gameContext->getPhysics()->CreateStaticRigidBody(m_cacheWorldMatrix, &bodySetup);

		if (hPhysicsBody.isValid())
		{
			IPhysicsBody* pPhysicsBody = hPhysicsBody.getObject<IPhysicsBody>();
			pPhysicsBody->setGameObject(this);
			pPhysicsBody->setCollisionGroup(COLLISION_STATIC);
			pPhysicsBody->enableCollisionGroups(COLLISION_STATIC | COLLISION_DYNAMIC);
			pPhysicsBody->setTriggerOnly(m_bTrigger);
			pPhysicsBody->setupPhysicsBody();
		}
	}
	
}