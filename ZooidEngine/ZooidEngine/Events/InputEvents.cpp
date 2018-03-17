#include "InputEvents.h"

#include "Utils/Array.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(Event_INPUT, Event)
	IMPLEMENT_CLASS_1(Event_KEY_DOWN, Event_INPUT)
	IMPLEMENT_CLASS_1(Event_KEY_UP, Event_INPUT)
	IMPLEMENT_CLASS_1(Event_MOUSE_DRAG, Event_INPUT)
}