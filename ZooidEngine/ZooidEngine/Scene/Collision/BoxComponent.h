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

		// CollisionComponent implementation
		virtual void setupPhysics() override;

		void setHalfExtent(const Vector3& _halfExtent)
		{
			m_halfExtent = _halfExtent;
		}

		FORCEINLINE Vector3 getHalfExtent() const { return m_halfExtent; }

	private:
		Vector3 m_halfExtent;
	};
}
#endif
