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

		// Collision implementation
		virtual void setupPhysics() override;

		void setRadius(Float32 _radius) { m_radius = _radius; }
		void setHeight(Float32 _height) { m_height = _height; }

		FORCEINLINE Float32 getRadius() const { return m_radius; }
	 	FORCEINLINE Float32 getHeight() const { return m_height; }

	private:
		Float32 m_radius;
		Float32 m_height;
	};
}
#endif
