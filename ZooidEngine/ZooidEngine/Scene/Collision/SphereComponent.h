#ifndef __ZE_SPHERE_COMPONENT_H__
#define __ZE_SPHERE_COMPONENT_H__

#include "CollisionComponent.h"

namespace ZE
{
	class SphereComponent : public CollisionComponent
	{
		DEFINE_CLASS(SphereComponent)

	public:
		SphereComponent(GameContext* gameContext) : CollisionComponent(gameContext)
		{}

		virtual ~SphereComponent() {}

		virtual void setupPhysics() override;
		
		Float32 m_radius;
	};
}
#endif
