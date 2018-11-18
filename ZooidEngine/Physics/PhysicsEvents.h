#ifndef __PHYSICS_EVENTS_H__
#define __PHYSICS_EVENTS_H__

#include "Events/Events.h"
#include "Math/Matrix4x4.h"
#include "Physics.h"

namespace ZE
{
	class Component;

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

	public:

		Component* m_component;
		Component* m_otherComponent;

		Array<ContactPhysicsData> m_contacts;
	};

	class Event_Physics_UPDATE_TRANSFORM : public Event_Physics_BASE
	{
		DEFINE_CLASS(Event_Physics_UPDATE_TRANSFORM);

	public:

		ALIGN16 Matrix4x4 m_worldTransform;
	};

	class Event_Physics_ON_TRIGGER : public Event_Physics_BASE
	{
		DEFINE_CLASS(Event_Physics_ON_TRIGGER);

	public:

		Component* m_triggerComponent;
		Component* m_otherComponent;
	};

	class Event_Physics_ON_BEGIN_TRIGGER : public Event_Physics_ON_TRIGGER
	{
		DEFINE_CLASS(Event_Physics_ON_BEGIN_TRIGGER);
	};

	class Event_Physics_ON_END_TRIGGER : public Event_Physics_ON_TRIGGER
	{
		DEFINE_CLASS(Event_Physics_ON_END_TRIGGER);
	};

}
#endif
