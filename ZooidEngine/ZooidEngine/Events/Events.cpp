#include "Events.h"

#include "../Utils/Array.h"

namespace ZE {
	
	IMPLEMENT_CLASS_0(Event);
	IMPLEMENT_CLASS_1(Event_UPDATE, Event);
	IMPLEMENT_CLASS_1(Event_GATHER_LIGHT, Event);
	IMPLEMENT_CLASS_1(Event_RESOURCE_LOADED, Event);

}
