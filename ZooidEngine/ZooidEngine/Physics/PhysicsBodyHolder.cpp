#include "PhysicsBodyHolder.h"

#include "PhysicsBody.h"

namespace ZE
{

	void IPhysicsBodyHolder::AddForceAtPos(const Vector3& globalPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse /*= false*/)
	{
		if (hasPhysicsBody())
		{
			IPhysicsBody* pPhysicsBody = getPhysicsBody();
			pPhysicsBody->AddForceAtPos(globalPos, forceDir, forceValue, bImpulse);
		}
	}

	void IPhysicsBodyHolder::AddForceAtLocalPos(const Vector3& localPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse /*= false*/)
	{
		if (hasPhysicsBody())
		{
			IPhysicsBody* pPhysicsBody = getPhysicsBody();
			pPhysicsBody->AddForceAtLocalPos(localPos, forceDir, forceValue, bImpulse);
		}
	}

}