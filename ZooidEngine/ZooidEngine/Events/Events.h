#ifndef __ZE_EVENTS__
#define __ZE_EVENTS__

#include "GameObjectModel/Object.h"

namespace ZE {
	
	class GameContext;

	class Event : public Object
	{
		DEFINE_CLASS(Event)

	public:
		Event() {}
		virtual ~Event() {}

	};

	class Event_UPDATE : public Event
	{
		DEFINE_CLASS(Event_UPDATE)

	public:
		Event_UPDATE() {}
		virtual ~Event_UPDATE() {}

		Float32 m_deltaTime;
	};
};

#endif 
