#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Memory/Handle.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Utils/PrimitiveTypes.h"
#include "Utils/Array.h"

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
		HEIGHT_FIELDS
	};

	struct PhysicsBodyDesc
	{
		PhysicsBodyDesc() {}
		~PhysicsBodyDesc() {}

		PhysicsShape ShapeType;

		Matrix4x4 Transform;
		Float32 Mass;

		union
		{
			Vector3 HalfExtent;
			struct
			{
				Float32 Radius;
				Float32 HalfHeight;
			};

			void* MeshData;
		};
	};

	class PhysicsBodySetup
	{
	public:
		PhysicsBodySetup() {}

		Array<PhysicsBodyDesc, true> m_bodies;
	};


	class IPhysics
	{
	public:

		IPhysics() {}
		virtual ~IPhysics() {}

		virtual void Setup() = 0;
		virtual void PreUpdate() = 0;
		virtual void Update(float _deltaMS) = 0;
		virtual void PostUpdate() = 0;
		virtual void DrawDebug() = 0;
		virtual void Destroy() = 0;

		virtual Handle CreateDynamicRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup) = 0;
		virtual Handle CreateStaticRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup) = 0;

		virtual void DestroyPhysicsObject(Handle handle) = 0;
	};
}
#endif
