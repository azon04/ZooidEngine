#ifndef __PHYSICS_ZOOID__
#define __PHYSICS_ZOOID__

#include "Zooid/BaseZooid.h"
#include "Utils/Macros.h"
#include "Memory/Handle.h"

namespace ZE
{
	class IPhysics;

	class PhysicsZooid : public BaseZooid
	{
	public:

		PhysicsZooid(GameContext* gameContext) : BaseZooid(gameContext)
		{}


		FORCEINLINE IPhysics* GetPhysics() const { return m_physics; }

		// Need to implement when we implemenenting specific Physics Engine
		static PhysicsZooid* GetPhysicsZood(GameContext* gameContext);

	protected:
		IPhysics* m_physics;
	};
}
#endif
