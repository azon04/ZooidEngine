#ifndef __PHYSX_ZOOID__
#define __PHYSX_ZOOID__

#include "Physics/PhysicsZooid.h"

namespace ZE
{
	class PhysXZooid : public PhysicsZooid
	{
	public:

		PhysXZooid(GameContext* gameContext) : PhysicsZooid(gameContext)
		{}

		virtual void Init() override;
		virtual void Destroy() override;

	};
}
#endif
