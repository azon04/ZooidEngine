#ifndef __PHYSX_H__
#define __PHYSX_H__

#include "Physics/Physics.h"
#include "PxSimulationEventCallback.h"

namespace physx
{
	class PxFoundation;
	class PxPhysics;
	class PxDefaultCpuDispatcher;
	class PxScene;
	class PxPvd;
	class PxShape;
	class PxMaterial;
}

namespace ZE
{
	class PhysXEngine : public IPhysics, public physx::PxSimulationEventCallback
	{

	public:
		
		// IPhysics Implementation
		virtual void Setup() override;
		virtual void PreUpdate() override;
		virtual void Update(float _deltaSeconds) override;
		virtual void PostUpdate() override;
		virtual void DrawDebug() override;
		virtual void Destroy() override;

		virtual Handle CreateDynamicRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup) override;
		virtual Handle CreateStaticRigidBody(Matrix4x4& _transform, PhysicsBodySetup* _setup) override;

		virtual void DestroyPhysicsObject(Handle handle) override;
		
		// Scene Queries
		virtual bool DoLineRaycast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) override;
		virtual bool DoLineRaycastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) override;
		
		virtual bool DoBoxCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponets = sDefaultIgnoredComponent) override;
		virtual bool DoBoxCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, const Vector3& halfExtent, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) override;
		
		virtual bool DoSphereCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) override;
		virtual bool DoSphereCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, Float32 radius, PhysicsHit& hit, Array<Component*, true>& ignoredComponents = sDefaultIgnoredComponent) override;
		
		virtual bool DoCapsuleCast(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoreComponents) override;
		virtual bool DoCapsuleCastMulti(UInt32 _groups, const Vector3& startPos, const Vector3& dir, Float32 distance, const Quaternion& quat, Float32 radius, Float32 halfHeight, PhysicsHit& hit, Array<Component*, true>& ignoredComponents) override;
		
		// Overlaps
		virtual bool BoxOverlap(const Vector3& position, const Quaternion& quat, const Vector3& halfExtent, Array<IPhysicsBody*, true>& physicsBodyResult) override;
		virtual bool SphereOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Array<IPhysicsBody*, true>& physicsBodyResult) override;
		virtual bool CapsuleOverlap(const Vector3& position, const Quaternion& quat, Float32 radius, Float32 halfHeight, Array<IPhysicsBody*, true>& physicsBodyResult) override;
		virtual bool PhysicsBodyOverlap(IPhysicsBody* physicsBody, Array<IPhysicsBody*, true>& physicsBodyResult) override;
		// End IPhysics Implementation

		// Implement from PxSimulationEventCallback
		virtual void onConstraintBreak(physx::PxConstraintInfo* constraints, physx::PxU32 count);
		virtual void onWake(physx::PxActor** actors, physx::PxU32 count);
		virtual void onSleep(physx::PxActor** actors, physx::PxU32 count);
		virtual void onContact(const physx::PxContactPairHeader& pairHeader, const physx::PxContactPair* pairs, physx::PxU32 nbPairs);
		virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count);
		virtual void onAdvance(const physx::PxRigidBody*const* bodyBuffer, const physx::PxTransform* poseBuffer, const physx::PxU32 count);

	private:
		physx::PxShape* CreateShape(PhysicsBodyDesc* _data, Vector3 scale);

		physx::PxFoundation* m_physxFoundation;
		physx::PxPhysics* m_physxPhysics;
		physx::PxDefaultCpuDispatcher* m_physxCPUDispatcher;
		physx::PxScene* m_physxScene;
		physx::PxPvd* m_physxPVD;
		physx::PxMaterial* m_defaultPhysicsMaterial;
	};
}
#endif
