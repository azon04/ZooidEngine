#ifndef __ZE_EVENTS__
#define __ZE_EVENTS__

#include "GameObjectModel/Object.h"
#include "Memory/Handle.h"

namespace ZE {
	
	class GameContext;

	class Event : public Object
	{
		DEFINE_CLASS(Event)

	public:
		Event() : m_bPropagate(true) {}
		virtual ~Event() {}

		bool m_bPropagate;
	};

	class Event_UPDATE : public Event
	{
		DEFINE_CLASS(Event_UPDATE)

	public:
		Event_UPDATE() {}
		virtual ~Event_UPDATE() {}

		// Delta Time in milliseconds
		double m_deltaTime;
	};

	class Event_GATHER_LIGHT : public Event
	{
		DEFINE_CLASS(Event_GATHER_LIGHT);

	public: 
		Event_GATHER_LIGHT() {}
		virtual ~Event_GATHER_LIGHT() {}
	};

	class Event_GATHER_RENDER : public Event
	{
		DEFINE_CLASS(Event_GATHER_RENDER);

	public:
		Event_GATHER_RENDER() {}
		virtual ~Event_GATHER_RENDER() {}
	};

	class Event_RESOURCE_LOADED : public Event
	{
		DEFINE_CLASS(Event_RESOURCE_LOADED)

	public:
		Event_RESOURCE_LOADED() {}
		virtual ~Event_RESOURCE_LOADED() {}

		Handle m_resourceHandle;
	};
};

#endif 
