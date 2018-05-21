#ifndef __ZE_CAPSULE_COMPONENT_H__
#define __ZE_CAPSULE_COMPONENT_H__

#include "CollisionComponent.h"

namespace ZE
{
	class CapsuleComponent : public CollisionComponent
	{
		DEFINE_CLASS(CapsuleComponent)

	public:
		CapsuleComponent(GameContext* gameContext) : CollisionComponent(gameContext)
		{}
		virtual ~CapsuleComponent() {}

		virtual void setupPhysics() override;

		Float32 m_radius;
		Float32 m_height;
	};
}
#endif
