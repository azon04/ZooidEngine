#ifndef __PHYSX_BODY_H__
#define __PHYSX_BODY_H__

#include "Physics/PhysicsBody.h"

#include "PxPhysicsAPI.h"

namespace ZE
{

	class PhysXBody : public IPhysicsBody
	{

	public:
		PhysXBody(physx::PxRigidActor* _rigidActor);

		physx::PxRigidActor* getRigidActor() const { return m_physXRigidActor; }

		virtual void setupPhysicsBody() override;

		virtual void setMass(Float32 _mass) override;
		virtual Float32 getMass() const override;

		virtual void AddForceAtPos(const Vector3& globalPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse = false);
		virtual void AddForceAtLocalPos(const Vector3& localPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse = false);
		
	private:
		physx::PxRigidActor* m_physXRigidActor;
		physx::PxFilterData m_filterData;
	};
}

#endif
