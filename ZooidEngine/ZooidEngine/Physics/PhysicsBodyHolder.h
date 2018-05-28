#ifndef __PHYSICS_BODY_HOLDER_H__
#define __PHYSICS_BODY_HOLDER_H__

#include "Math/Vector3.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	class IPhysicsBody;

	class IPhysicsBodyHolder
	{

	public:

		virtual bool hasPhysicsBody() = 0;
		virtual IPhysicsBody* getPhysicsBody() = 0;

		virtual void AddForceAtPos(const Vector3& globalPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse = false);
		virtual void AddForceAtLocalPos(const Vector3& localPos, const Vector3& forceDir, Float32 forceValue, bool bImpulse = false);
	};
}
#endif
