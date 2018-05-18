#ifndef __PHYSICS_EVENTS_H__
#define __PHYSICS_EVENTS_H__

#include "Events/Events.h"
#include "Math/Matrix4x4.h"

namespace ZE
{
	class Event_Physics_BASE : public Event
	{
		DEFINE_CLASS(Event_Physics_BASE)

	public:
		Event_Physics_BASE()
		{
			m_bPropagate = false;
		}
	};

	class Event_Physics_PREUPDATE : public Event_Physics_BASE
	{
		DEFINE_CLASS(Event_Physics_PREUPDATE)
	};

	class Event_Physics_UPDATE : public Event_Physics_BASE
	{
		DEFINE_CLASS(Event_Physics_UPDATE)
	};

	class Event_Physics_POSTUPDATE : public Event_Physics_BASE
	{
		DEFINE_CLASS(Event_Physics_POSTUPDATE);
	};

	class Event_Physics_ONCOLLIDE : public Event_Physics_BASE
	{
		DEFINE_CLASS(Event_Physics_ONCOLLIDE);
	};

	class Event_Physics_UPDATE_TRANSFORM : public Event_Physics_BASE
	{
		DEFINE_CLASS(Event_Physics_UPDATE_TRANSFORM);

	public:

		Matrix4x4 m_worldTransform;
	};

}
#endif
