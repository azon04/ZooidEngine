#ifndef __ZE_EVENT_DISPATCHER__
#define __ZE_EVENT_DISPATCHER__

#include "GameObjectModel/Component.h"

namespace ZE
{
	enum EventType
	{
		EVENT_GENERAL,
		EVENT_INPUT
	};

	class EventDispatcher : public Component
	{
		DEFINE_CLASS(EventDispatcher)

	public:
		EventDispatcher(GameContext* gameContext) :
			Component(gameContext)
		{}

		void addEvent(Handle _eventHandle, EventType _type = EVENT_GENERAL);
		void clearEvents(EventType _type);

		FORCEINLINE Array<Handle, true>& getEvents(EventType _type)
		{
			if (_type == EVENT_INPUT)
			{
				return m_inputEvents;
			}

			return m_generalEvents;
		}

	protected:
		Array<Handle, true> m_generalEvents;
		Array<Handle, true> m_inputEvents;
	};
};
#endif
