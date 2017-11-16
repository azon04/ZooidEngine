#ifndef __ZE_EVENT_DISPATCHER__
#define __ZE_EVENT_DISPATCHER__

#include "GameObjectModel/Component.h"

namespace ZE
{
	class EventDispatcher : public Component
	{
		DEFINE_CLASS(EventDispatcher)

	public:
		EventDispatcher(GameContext* gameContext) :
			Component(gameContext)
		{}
	};
};
#endif
