#ifndef __PHYSICS_BODY_H__
#define __PHYSICS_BODY_H__

#include "Physics.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	class Object;

	enum CollisionGroup : UInt32
	{
		COLLISION_STATIC = (1 << 0),
		COLLISION_DYNAMIC = (1 << 1),
		COLLISION_CUSTOM_1 = (1 << 2),
		COLLISION_CUSTOM_2 = (1 << 3),
		COLLISION_CUSTOM_3 = (1 << 4)
	};

	class IPhysicsBody
	{
	public:
		IPhysicsBody() : 
			m_bCollisionEnabled(true), 
			m_bGenerateTouchEvent(false), 
			m_bTriggerOnly(false), 
			m_bEnableGravity(true),
			m_mass(0.0) {}

		// Enable collision
		virtual void setEnableCollision(bool _bCollisionEnabled);

		// Disable collision
		virtual void setCollisionGroup(UInt32 _collisionGroup);
		
		// enable collision groups
		virtual void enableCollisionGroups(UInt32 _CollisionGroups);
		
		// disable certain collision groups
		virtual void disableCollisionGroups(UInt32 _CollisionGroups);
		
		// Set this physics body as trigger only (ghost physics body)
		virtual void setTriggerOnly(bool _bTriggerOnly);

		// Enable/Disable grapvity for this physics body
		virtual void setEnableGravity(bool _bEnableGravity);

		// Set mass of this physics body
		virtual void setMass(float _mass);

		// Enable/disable generateTouchEvent for this object
		virtual void setGenerateTouchEvent(bool _bEnableTouchEvent);

		virtual void setupPhysicsBody() {};

		// Get Game object of the physics body
		Object* getGameObject() { return m_gameObject; }

		// Set the game object of the physics body
		void setGameObject(Object* _gameObject) { m_gameObject = _gameObject; }

		// Get Mass
		virtual Float32 getMass() const { return m_mass; }

		// Applying forces
		// Add force at world position
		virtual void AddForceAtPos(const Vector3& globalPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse = false) = 0;

		// Add force at local position of the physics body
		virtual void AddForceAtLocalPos(const Vector3& localPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse = false) = 0;

	protected:
		Object* m_gameObject;

		UInt32 m_collisionGroup;
		UInt32 m_collisionGroupsMask;
		bool m_bGenerateTouchEvent;
		bool m_bCollisionEnabled;
		bool m_bTriggerOnly;
		bool m_bEnableGravity;
		Float32 m_mass;
	};
}
#endif
