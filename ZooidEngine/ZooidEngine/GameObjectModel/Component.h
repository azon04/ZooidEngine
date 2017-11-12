#ifndef __ZE_COMPONENT_H__
#define __ZE_COMPONENT_H__

// Outer Engine includes

#include "Utils/Array.h"
#include "Utils/HashMap.h"
#include "Object.h"

#include "MemoryManagement/Handle.h"
#include "GameObjectModel/FunctionDelegates.h"

namespace ZE {
	class GameContext;
	class Event;

	class Component : public Object {

		DEFINE_CLASS(Component)

	public:
		Component(GameContext* gameContext) : m_gameContext(gameContext){}
		virtual ~Component() {}

		void handleEvent(Event* event);
		void addChild(Component* child);

		virtual void setupComponent() {}

	protected:


		void addEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc);
		void removeEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc);

	private:
		void registerEventToParent(Int32 classId);
		void unregisterEventFromParent(Int32 classId);
		bool registerChildToEvent(Int32 classId, Component* child);
		bool unregisterChildFromEvent(Int32 classId, Component* child);

	public:
		Array<Component*, true> m_components;
		Array<Component*, true> m_parents;

	protected:
		GameContext* m_gameContext;

	private:
		HashMap<Int32, Array<Component*, true>> m_eventMap;
		HashMap<Int32, Array<EventDelegate, true>> m_delegateMap;
	};
}
#endif
