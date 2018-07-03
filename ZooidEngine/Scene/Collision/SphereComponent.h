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

		// CollisionComponent  implementation
		virtual void setupPhysics() override;
		
		void setRadius(Float32 _radius) { m_radius = _radius; }
		FORCEINLINE Float32 getRadius() const { return m_radius; }
	private:
		Float32 m_radius;
	};
}
#endif
