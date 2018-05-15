#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Memory/Handle.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	enum PhysicsShape : UInt16
	{
		BOX,
		SPHERE,
		CAPSULE,
		PLANE,
		CONVEX_MESHES,
		TRIANGLE_MESHES,
		HEIGHT_FIELDS,
		COMPOUND_SHAPE
	};

	class IPhysics
	{
	public:

		IPhysics() {}
		virtual ~IPhysics() {}

		virtual void Setup() = 0;
		virtual void PreUpdate() = 0;
		virtual void Update() = 0;
		virtual void PostUpdate() = 0;
		virtual void DrawDebug() = 0;
		virtual void Destroy() = 0;

		virtual Handle CreateTriggerCollision(PhysicsShape _shape, void* _data) = 0;
		virtual Handle CreateDynamicRigidBody(PhysicsShape _shape, void* _data) = 0;
		virtual Handle CreateStaticRigidBody(PhysicsShape _shape, void* _data) = 0;

		virtual void DestroyPhysicsObject(Handle handle) = 0;
	};
}
#endif
