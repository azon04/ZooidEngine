#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Memory/Handle.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Utils/PrimitiveTypes.h"
#include "Utils/Array.h"

namespace ZE
{
	class Component;
	class IPhysicsBody;

	enum PhysicsShape : UInt16
	{
		NONE,
		BOX,
		SPHERE,
		CAPSULE,
		PLANE,
		CONVEX_MESHES,
		TRIANGLE_MESHES,
		HEIGHT_FIELDS
	};

	struct ContactPhysicsData
	{
		ContactPhysicsData() {}
		~ContactPhysicsData() {}

		Component* m_component;
		Component* m_otherComponent;
		Vector3 m_contactPos;
		Vector3 m_contactNormal;
		Vector3 m_contactImpulse;
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

	struct PhysicsHit
	{
		PhysicsHit() {}
		~PhysicsHit() {}

		bool isBlocked;
		Component* blockComponent = nullptr;
		Vector3 blockPosition;
		Vector3 blockNormal;

		bool hasTouches = false;
		Array<PhysicsHit, true> touches;
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

		// Queries
		virtual bool DoLineRaycast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		virtual bool DoLineRaycastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		virtual bool DoBoxCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		virtual bool DoBoxCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		virtual bool DoSphereCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		virtual bool DoSphereCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		virtual bool DoCapsuleCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		virtual bool DoCapsuleCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// overlaps
		virtual bool BoxOverlap(const Vector3& position, const Quaternion& quat, const Vector3& halfExtent, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;
		virtual bool SphereOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;
		virtual bool CapsuleOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Float32 halfHeight, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;
		virtual bool PhysicsBodyOverlap(IPhysicsBody* physicsBody, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;
		
		virtual void DestroyPhysicsObject(Handle handle) = 0;

	protected:

		static Array<Component*, true> sDefaultIgnoredComponent;
	};

	__declspec(selectany) ZE::Array<Component*, true> IPhysics::sDefaultIgnoredComponent;

}
#endif
