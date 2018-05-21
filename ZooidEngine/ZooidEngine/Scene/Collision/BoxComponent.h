#ifndef __BOX_COMPONENT_H__
#define __BOX_COMPONENT_H__

#include "CollisionComponent.h"

namespace ZE
{
	class BoxComponent : public CollisionComponent
	{
		DEFINE_CLASS(BoxComponent);

	public:
		BoxComponent(GameContext* gameContext) : CollisionComponent(gameContext)
		{}
		virtual ~BoxComponent() {}

		virtual void setupPhysics() override;

		Vector3 m_halfExtent;
	};
}
#endif
