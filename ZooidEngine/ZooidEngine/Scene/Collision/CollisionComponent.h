#ifndef __ZE_COLLISION_COMPONENT_H__
#define __ZE_COLLISION_COMPONENT_H__

#include "Scene/SceneComponent.h"

namespace ZE
{
	class Event;

	class CollisionComponent : public SceneComponent
	{
		DEFINE_CLASS(CollisionComponent)

	public:
		CollisionComponent(GameContext* gameContext) : SceneComponent(gameContext) {}
		virtual ~CollisionComponent() {}

		virtual void setupComponent() override;
		virtual void setupPhysics();

		void setStatic(bool _bStatic) { m_bStatic = _bStatic; }
		void setTrigger(bool _bTrigger) { m_bTrigger = _bTrigger; }

	protected:

		virtual void handlePhysicsUpdateTransform(Event* pEvent);
		virtual void handleOnCollide(Event* pEvent);
		virtual void handleOnBeginTrigger(Event* pEvent);
		virtual void handleOnEndTrigger(Event* pEvent);

		bool m_bStatic;
		bool m_bTrigger;
		Handle hPhysicsBody;
	};
}
#endif
