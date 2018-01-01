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

		EventDelegate()
			: m_object(nullptr), m_eventFunc(nullptr)
		{}

		EventDelegate(Object* Obj, HandleEventFunc eventFunc)
			: m_object(Obj), m_eventFunc(eventFunc)
		{}

		void call(Event* event)
		{
			if (m_object && m_eventFunc)
			{
				(m_object->*m_eventFunc)(event);
			}
		}

		bool operator==(const EventDelegate& other)
		{
			return m_object == other.m_object
				&& m_eventFunc == other.m_eventFunc;
		}

		HandleEventFunc m_eventFunc = NULL;
		Object* m_object = NULL;
	};

	class NativeEventDelegate
	{
	public:
		typedef void(*HandleEventFunc)(Event*);

		NativeEventDelegate(HandleEventFunc eventFunc)
			: m_eventFunc(eventFunc)
		{

		}

		void call(Event* event)
		{
			if (m_eventFunc)
			{
				m_eventFunc(event);
			}
		}

		HandleEventFunc m_eventFunc = NULL;
	};
};
#endif
