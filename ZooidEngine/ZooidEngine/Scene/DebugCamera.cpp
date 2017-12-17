#include "DebugCamera.h"

#include "Events/InputEvents.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(DebugCamera, CameraComponent);

	void DebugCamera::setupComponent()
	{
		CameraComponent::setupComponent();
		addEventDelegate(Event_KEY_UP, &DebugCamera::handleKeyUpEvent);
		addEventDelegate(Event_KEY_DOWN, &DebugCamera::handleKeyDownEvent);
	}

	void DebugCamera::handleUpdateEvent(Event* _event)
	{
		ZEINFO("Event Handle Overrided");
	}

	void DebugCamera::handleKeyDownEvent(Event* _event)
	{
		Event_INPUT* pRealEvent = (Event_INPUT*)_event;
		if (pRealEvent->m_keyId == 'W')
		{
			m_velocity.setZ(m_velocity.getZ() + 1.0f);
		}
	}

	void DebugCamera::handleKeyUpEvent(Event* _event)
	{
		Event_INPUT* pRealEvent = (Event_INPUT*)_event;
		if (pRealEvent->m_keyId == 'W')
		{
			m_velocity.setZ(m_velocity.getZ() - 1.0f);
		}
	}



}