#include "PhysicsBody.h"

namespace ZE
{

	void IPhysicsBody::setEnableCollision(bool _bCollisionEnabled)
	{
		m_bCollisionEnabled = _bCollisionEnabled;
	}

	void IPhysicsBody::setCollisionGroup(UInt32 _collisionGroup)
	{
		m_collisionGroup = _collisionGroup;
	}

	void IPhysicsBody::enableCollisionGroups(UInt32 _CollisionGroups)
	{
		m_collisionGroupsMask |= _CollisionGroups;
	}

	void IPhysicsBody::disableCollisionGroups(UInt32 _CollisionGroups)
	{
		m_collisionGroupsMask &= (~_CollisionGroups);
	}

	void IPhysicsBody::setGenerateTouchEvent(bool _bEnableTouchEvent)
	{
		m_bGenerateTouchEvent = _bEnableTouchEvent;
	}

}

