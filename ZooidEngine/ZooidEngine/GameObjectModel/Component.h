#ifndef __ZE_COMPONENT_H__
#define __ZE_COMPONENT_H__

#include "Utils/Array.h"
#include "Utils/HashMap.h"

#include "Object.h"
#include "FunctionDelegates.h"

#define addEventDelegate(EventClass, Func) _addEventDelegate(EventClass::GetClassID(), (ZE::EventDelegate::HandleEventFunc) Func);
#define removeEventDelegate(EventClass, Func) _removeEventDelegate(EventClass::GetClassID(), (ZE::EventDelegate::HandleEventFunc) Func);

namespace ZE 
{
	class GameContext;
	class Event;

	class Component : public Object 
	{

		DEFINE_CLASS(Component)

	public:
		Component(GameContext* gameContext) : m_gameContext(gameContext) {}
		virtual ~Component() {}

		// Handle event. 
		// Call function that handle certain event and pass it to children if the event is supposed to be propagated
		void handleEvent(Event* event);

		// Add another component as a child of this component
		void addChild(Component* child);

		// Remove a component from children list
		void removeChild(Component* child);

		// Remove self from parent
		void removeFromParents();

		// Find first component from children list with class id
		Component* findFirstCompClassId(Int32 classId);

		// Find first component from children list with class id
		template<class T>
		T* findFirstCompByClassId(Int32 classId)
		{
			return (T*)findFirstCompClassId(classId);
		}

		virtual void setupComponent() {}

		// Register event function to handle a specific event
		void _addEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc);
		
		// Remove event function from list of event handles
		void _removeEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc);

	private:
		void registerEventToParent(Int32 classId);
		void unregisterEventFromParent(Int32 classId);
		bool registerChildToEvent(Int32 classId, Component* child);
		bool unregisterChildFromEvent(Int32 classId, Component* child);

	protected:
		Array<Component*, true> m_components;
		Array<Component*, true> m_parents;

		GameContext* m_gameContext;

	private:
		HashMap<Int32, Array<Component*, true>> m_eventMap;
		HashMap<Int32, Array<EventDelegate, true>> m_delegateMap;
	};
}
#endif
