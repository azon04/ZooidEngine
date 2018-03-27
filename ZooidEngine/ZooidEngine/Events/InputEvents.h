#ifndef __ZE_INPUT_EVENTS_H__
#define __ZE_INPUT_EVENTS_H__

#include "Events.h"

namespace ZE
{
	class GameContext;

	class Event_INPUT : public Event
	{
		DEFINE_CLASS(Event_INPUT)
	
	public:
		Event_INPUT() {}
		virtual ~Event_INPUT() {}

		Short m_keyId;
	};

	class Event_KEY_UP : public Event_INPUT
	{
		DEFINE_CLASS(Event_KEY_UP)

	public:
		Event_KEY_UP() {}
		virtual ~Event_KEY_UP() {}
	};

	class Event_KEY_DOWN : public Event_INPUT
	{
		DEFINE_CLASS(Event_KEY_DOWN)

	public:
		Event_KEY_DOWN() {}
		virtual ~Event_KEY_DOWN() {}
	};

	class Event_MOUSE_DRAG : public Event_INPUT
	{
		DEFINE_CLASS(Event_MOUSE_DRAG)
	
	public:
		Event_MOUSE_DRAG() {}
		virtual ~Event_MOUSE_DRAG() {}

		int m_deltaX;
		int m_deltaY;
	};
}

#endif