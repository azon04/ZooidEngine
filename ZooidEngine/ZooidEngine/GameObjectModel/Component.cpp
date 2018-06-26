#include "Component.h"
#include "Events/Events.h"

namespace ZE {
	IMPLEMENT_CLASS_0(Component)

	// Return true if child added to event map
	bool Component::registerChildToEvent(Int32 classId, Component* child)
	{
		if (!m_eventMap.hasKey(classId))
		{
			m_eventMap.put(classId, Array<Component*, true>());
		}

		if (!m_eventMap[classId].contains(child))
		{
			m_eventMap[classId].push_back(child);
			return true;
		}
		else
		{
			return false;
		}
	}

	// Return true if child removed from event map
	bool Component::unregisterChildFromEvent(Int32 classId, Component* child)
	{
		if (!m_eventMap.hasKey(classId))
		{
			return false;
		}

		int index = m_eventMap[classId].firstIndexOf(child);
		if (index == -1)
		{
			return false;
		}
		
		m_eventMap[classId].removeAt(index);
		return true;
	}

	void Component::registerEventToParent(Int32 classId)
	{
		for (int i = 0; i < m_parents.length(); i++)
		{
			if (m_parents[i]->registerChildToEvent(classId, this))
			{
				m_parents[i]->registerEventToParent(classId);
			}
		}
	}

	void Component::unregisterEventFromParent(Int32 classId)
	{
		for (int i = 0; i < m_parents.length(); i++)
		{
			if (m_parents[i]->unregisterChildFromEvent(classId, this))
			{
				//  check if event map is zero (means there is no child for this event) and check if current object has no delegate map for this function
				if (m_parents[i]->m_eventMap[classId].length() == 0 && !m_parents[i]->m_delegateMap.hasKey(classId))
				{
					// removing the event map
					m_parents[i]->m_eventMap.erase(classId);
					m_parents[i]->unregisterEventFromParent(classId);
				}
			}
		}
	}

	void Component::handleEvent(Event* event)
	{
		// To do check delegates
		Array<EventDelegate, true>& delegates = m_delegateMap[event->getClassID()];
		for (int i = 0; i < delegates.length(); i++)
		{
			delegates[i].call(event);
		}

		if (!event->m_bPropagate)
		{
			return;
		}

		Array<Component*, true>& components = m_eventMap[event->getClassID()];
		for (int i = 0; i < components.length(); i++)
		{
			components[i]->handleEvent(event);
		}
	}

	void Component::addChild(Component* child)
	{
		ZASSERT(!this->m_parents.contains(child), "THIS IS CIRCULAR RELATION");

		if (!child->m_parents.contains(this))
		{
			child->m_parents.push_back(this);
		}

		if (!this->m_components.contains(child))
		{
			this->m_components.push_back(child);
		}

		// to do set events handling to parent
		Array<Int32, true> eventIds = child->m_delegateMap.getKeys();
		for (int i = 0; i < eventIds.length(); i++)
		{
			registerChildToEvent(eventIds[i], child);
			registerEventToParent(eventIds[i]);
		}

		Array<Int32, true> addEventIds = child->m_eventMap.getKeys();
		for (int i = 0; i < addEventIds.length(); i++)
		{
			registerChildToEvent(addEventIds[i], child);
			registerEventToParent(addEventIds[i]);
		}
	}

	void Component::removeChild(Component* child)
	{
		int childIndex = this->m_components.firstIndexOf(child);
		if (childIndex == -1)
		{
			ZEINFO("Child Not Found");
			return;
		}

		// to do set events handling to parent
		Array<Int32, true> eventIds = child->m_delegateMap.getKeys();
		for (int i = 0; i < eventIds.length(); i++)
		{
			child->unregisterEventFromParent(eventIds[i]);
		}

		m_components.removeAt(childIndex);
		
		int parentIndex = child->m_parents.firstIndexOf(this);
		if (parentIndex >= 0)
		{
			child->m_parents.removeAt(parentIndex);
		}
	}

	void Component::removeFromParents()
	{
		for (int i = m_parents.length() - 1; i >= 0; i--)
		{
			m_parents[i]->removeChild(this);
		}
	}

	ZE::Component* Component::findFirstCompClassId(Int32 classId)
	{
		for (int i = 0; i < m_components.length(); i++)
		{
			if (m_components[i]->IsA(classId))
			{
				return m_components[i];
			}
		}
		return nullptr;
	}

	void Component::_addEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc)
	{
		if (!m_delegateMap.hasKey(eventId))
		{
			m_delegateMap.put(eventId, Array<EventDelegate, true>());
			registerEventToParent(eventId);
		}

		EventDelegate eventDelegate(this, eventFunc);
		if (!m_delegateMap[eventId].contains(eventDelegate))
		{
			m_delegateMap[eventId].push_back(eventDelegate);
		}

	}

	void Component::_removeEventDelegate(Int32 eventId, EventDelegate::HandleEventFunc eventFunc)
	{
		if (!m_delegateMap.hasKey(eventId))
		{
			return;
		}

		EventDelegate eventDelegate(this, eventFunc);
		int index = m_delegateMap[eventId].firstIndexOf(eventDelegate);
		if (index != -1)
		{
			m_delegateMap[eventId].removeAt(index);
		}

		if (m_delegateMap[eventId].length() == 0)
		{
			// clear mapping
			m_delegateMap.erase(eventId);
			// unregister event from parent
			unregisterEventFromParent(eventId);
		}
	}

}