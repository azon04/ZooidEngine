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

		FORCEINLINE bool isPropagate() const { return m_bPropagate; }

	protected:
		bool m_bPropagate;
	};

	class Event_UPDATE : public Event
	{
		DEFINE_CLASS(Event_UPDATE)

	public:
		Event_UPDATE() {}
		virtual ~Event_UPDATE() {}

		// Delta Time in milliseconds
		double m_deltaMilliseconds;

		// Delta Time in Seconds
		Float32 m_deltaSeconds;
	};

	class Event_GATHER_LIGHT : public Event
	{
		DEFINE_CLASS(Event_GATHER_LIGHT);

	public: 
		Event_GATHER_LIGHT() {}
		virtual ~Event_GATHER_LIGHT() {}
	};

	class Event_GATHER_BOUND : public Event
	{
		DEFINE_CLASS(Event_GATHER_BOUND);

	public:
		Event_GATHER_BOUND() {}
	};

	class Event_CALC_TRANSFORM : public Event
	{
		DEFINE_CLASS(Event_CALC_TRANSFORM);

	public:
		Event_CALC_TRANSFORM() {}
	};

	class Event_GATHER_SHADOW_LIST : public Event
	{
		DEFINE_CLASS(Event_GATHER_SHADOW_LIST);

	public:
		Event_GATHER_SHADOW_LIST() {}

		// Light Index
		Int32 m_shadowDataIndex = -1;
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

		// Resource handle of loaded resource
		Handle m_resourceHandle;
	};
};

#endif 
