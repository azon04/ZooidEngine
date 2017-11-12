#ifndef __ZE_FUNC_DELEGATES__
#define __ZE_FUNC_DELEGATES__

#include "Object.h"

namespace ZE
{
	class Event;

	class EventDelegate
	{
	public:
		typedef void(Object::*HandleEventFunc)(Event*);

		EventDelegate(Object* Obj, HandleEventFunc eventFunc)
			: m_object(Obj), m_eventFunc(eventFunc)
		{}

		void call(Event* event)
		{
			(m_object->*m_eventFunc)(event);
		}

		bool operator==(const EventDelegate& other)
		{
			return m_object == other.m_object
				&& m_eventFunc == other.m_eventFunc;
		}

		HandleEventFunc m_eventFunc;
		Object* m_object;
	};
};
#endif
