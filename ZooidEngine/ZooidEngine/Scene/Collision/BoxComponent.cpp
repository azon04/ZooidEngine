#include "BoxComponent.h"

#include "Utils/Array.h"
#include "Physics/Physics.h"
#include "Physics/PhysicsBody.h"
#include "ZEGameContext.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(BoxComponent, CollisionComponent);

	void BoxComponent::setupPhysics()
	{
		PhysicsBodySetup bodySetup;
		PhysicsBodyDesc bodyDesc;
		bodyDesc.ShapeType = BOX;
		bodyDesc.HalfExtent = m_halfExtent;
		bodySetup.m_bodies.push_back(bodyDesc);

		hPhysicsBody = m_gameContext->getPhysics()->CreateStaticRigidBody(m_worldTransform, &bodySetup);

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