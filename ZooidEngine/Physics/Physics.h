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

	// Contact physics data structure
	struct ContactPhysicsData
	{
		ContactPhysicsData() {}
		~ContactPhysicsData() {}

		// Main component that contact happen to
		Component* MainComponent;

		// Other/second component that contact happen to
		Component* OtherComponent;

		// Position of the contact
		Vector3 ContactPos;

		// Normal of the contact relative to Other component
		Vector3 ContactNormal;

		// Contact Impulse
		Vector3 ContactImpulse;
	};

	// Physics Body Desc structure: use to define and as parameter to creating PhysicsBody
	struct PhysicsBodyDesc
	{
		PhysicsBodyDesc() {}
		~PhysicsBodyDesc() {}

		// Physics shape type
		PhysicsShape ShapeType;

		// local transform of PhysicsBody
		ALIGN16 Matrix4x4 Transform;

		union
		{
			// Half extend for BOX
			Vector3 HalfExtent;
			struct
			{
				// Radius for SPHERE and CAPSULE
				Float32 Radius;
				// Half height for CAPSULE
				Float32 HalfHeight;
			};

			// Mesh data for CONVEX_MESHES, TRIANGLE_MESHES and HEIGHT_FIELDS
			void* MeshData;
		};
	};

	// Physics body setup class
	class PhysicsBodySetup
	{
	public:
		PhysicsBodySetup() {}

		// Mass for body setup
		Float32 Mass;

		// List of body desc
		Array<PhysicsBodyDesc, true> m_bodies;
	};

	// Physics Hit structure: Type return from cast
	struct PhysicsHit
	{
		PhysicsHit() {}
		~PhysicsHit() {}

		// whether the hit is blocked
		bool bIsBlocked;

		// Component that block the cast
		Component* BlockedComponent = nullptr;

		// the position where cast hit the blocked component -- in world space
		Vector3 BlockedPosition;

		// the normal where cast hit the blocked component
		Vector3 BlockedNormal;

		// Whether it has touched other object before of after the block happened, depending on cast
		bool bHasTouches = false;

		// List of touches
		Array<PhysicsHit, true> Touches;
	};

	// Main interface for Physics
	class IPhysics
	{
	public:

		IPhysics() {}
		virtual ~IPhysics() {}

		// Setup Physics
		virtual void Setup() = 0;

		// Preupdate before the actual physics phase happen
		virtual void PreUpdate() = 0;

		// Update the physics states
		virtual void Update(float _deltaSeconds) = 0;

		// PostUpdate after physics states changed
		virtual void PostUpdate() = 0;

		// Draw debug
		virtual void DrawDebug() = 0;

		// Destroy and unregister physics
		virtual void Destroy() = 0;

		// Create Dynamic Rigid body based on PhysicsBodySetup
		virtual Handle CreateDynamicRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup) = 0;

		// Create Static Rigid Body based on PhysicsBodySetup
		virtual Handle CreateStaticRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup) = 0;

		// Queries

		// Do single line raycast, stop when blocked
		virtual bool DoLineRaycast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// Do line raycast and collect all PhysicsHit that the cast overlap with
		virtual bool DoLineRaycastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// Do single box cast, stop when blocked
		virtual bool DoBoxCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// Do box cast and collect all PhysicsHit that the cast overlap with
		virtual bool DoBoxCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// Do single sphere cast, stop when blocked
		virtual bool DoSphereCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// Do sphere cast and collect all PhysicsHit that the cast overlap with
		virtual bool DoSphereCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// Do single capsule cast, stop when blocked
		virtual bool DoCapsuleCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// Do capsule cast and collect all PhysicsHit that the cast overlap with
		virtual bool DoCapsuleCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) = 0;
		
		// overlaps

		// Check and gather all overlap with the specific box
		virtual bool BoxOverlap(const Vector3& position, const Quaternion& quat, const Vector3& halfExtent, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;

		// Check and gather all overlap with the specific sphere
		virtual bool SphereOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;

		// Check and gather all overlap with the specific capsule
		virtual bool CapsuleOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Float32 halfHeight, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;

		// Check and gather all overlap with the specific physics body
		virtual bool PhysicsBodyOverlap(IPhysicsBody* physicsBody, Array<IPhysicsBody*, true>& physicsBodyResult) = 0;
		
		// Destroy physics object
		virtual void DestroyPhysicsObject(Handle handle) = 0;

	protected:

		static Array<Component*, true> sDefaultIgnoredComponent;
	};

	__declspec(selectany) ZE::Array<Component*, true> IPhysics::sDefaultIgnoredComponent;

}
#endif
