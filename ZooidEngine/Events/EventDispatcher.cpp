#include "EventDispatcher.h"

namespace ZE {
	IMPLEMENT_CLASS_1(EventDispatcher, Component)

	void EventDispatcher::addEvent(Handle _event, EventType _type /*= EVENT_GENERAL*/)
	{
		if (_type == EVENT_GENERAL)
		{
			m_generalEvents.push_back(_event);
		}
		else if (_type == EVENT_INPUT)
		{
			m_inputEvents.push_back(_event);
		}
	}

	void EventDispatcher::clearEvents(EventType _type)
	{
		if (_type == EVENT_GENERAL)
		{
			m_generalEvents.clear();
		}
		else if (_type == EVENT_INPUT)
		{
			m_inputEvents.clear();
		}
	}

};