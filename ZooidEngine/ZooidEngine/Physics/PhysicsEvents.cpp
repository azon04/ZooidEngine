#include "PhysicsEvents.h"

#include "Utils/Array.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(Event_Physics_BASE, Event);
	IMPLEMENT_CLASS_1(Event_Physics_PREUPDATE, Event_Physics_BASE);
	IMPLEMENT_CLASS_1(Event_Physics_POSTUPDATE, Event_Physics_BASE);
	IMPLEMENT_CLASS_1(Event_Physics_UPDATE, Event_Physics_BASE);
	IMPLEMENT_CLASS_1(Event_Physics_ONCOLLIDE, Event_Physics_BASE);
	IMPLEMENT_CLASS_1(Event_Physics_UPDATE_TRANSFORM, Event_Physics_BASE);
}