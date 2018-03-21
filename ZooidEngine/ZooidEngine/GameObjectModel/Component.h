#ifndef __ZE_COMPONENT_H__
#define __ZE_COMPONENT_H__

#include "Utils/Array.h"
#include "Utils/HashMap.h"
#include "Object.h"

#include "GameObjectModel/FunctionDelegates.h"

#define addEventDelegate(EventClass, Func) _addEventDelegate(EventClass::GetClassID(), (ZE::EventDelegate::HandleEventFunc) Func);
#define removeEventDelegate(EventClass, Func) _removeEventDelegate(EventClass::GetClassID(), (ZE::EventDelegate::HandleEventFunc) Func);

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
		void removeChild(Component* child);
		void removeFromParents();

		virtual void setupComponent() {}

		void _addEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc);
		void _removeEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc);

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
