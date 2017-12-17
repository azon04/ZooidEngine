#include "DebugCamera.h"

#include "Events/InputEvents.h"
#include "Input/KeyboardMouseInput.h"

// #TODO get rid of Windows here when we have Key abstraction
#include <Windows.h>

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
		m_worldTransform.setPos(m_worldTransform.getPos() + (m_worldTransform.getU() * m_velocity.getX() + m_worldTransform.getN() * m_velocity.getZ()) * 0.015f);
		m_worldTransform.rotateAroundU(DegToRad(m_rotation.getX() * 0.033f));
		m_worldTransform.rotateAroundV(DegToRad(m_rotation.getY() * 0.033f));
	}

	void DebugCamera::handleKeyDownEvent(Event* _event)
	{
		Event_INPUT* pRealEvent = (Event_INPUT*)_event;
		if (pRealEvent->m_keyId == 'W')
		{
			m_velocity.setZ(m_velocity.getZ() - 1.0f);
		}
		
		if (pRealEvent->m_keyId == 'S')
		{
			m_velocity.setZ(m_velocity.getZ() + 1.0f);
		}

		if (pRealEvent->m_keyId == 'A')
		{
			m_velocity.setX(m_velocity.getX() - 1.0f);
		}

		if (pRealEvent->m_keyId == 'D')
		{
			m_velocity.setX(m_velocity.getX() + 1.0f);
		}

		if (pRealEvent->m_keyId == VK_UP)
		{
			m_rotation.setX(m_rotation.getX() + 10.0f);
		}

		if (pRealEvent->m_keyId == VK_DOWN)
		{
			m_rotation.setX(m_rotation.getX() - 10.0f);
		}

		if (pRealEvent->m_keyId == VK_LEFT)
		{
			m_rotation.setY(m_rotation.getY() + 10.0f);
		}

		if (pRealEvent->m_keyId == VK_RIGHT)
		{
			m_rotation.setY(m_rotation.getY() - 10.0f);
		}
	}

	void DebugCamera::handleKeyUpEvent(Event* _event)
	{
		Event_INPUT* pRealEvent = (Event_INPUT*)_event;
		if (pRealEvent->m_keyId == 'W')
		{
			m_velocity.setZ(m_velocity.getZ() + 1.0f);
		}

		if (pRealEvent->m_keyId == 'S')
		{
			m_velocity.setZ(m_velocity.getZ() - 1.0f);
		}

		if (pRealEvent->m_keyId == 'A')
		{
			m_velocity.setX(m_velocity.getX() + 1.0f);
		}

		if (pRealEvent->m_keyId == 'D')
		{
			m_velocity.setX(m_velocity.getX() - 1.0f);
		}

		if (pRealEvent->m_keyId == VK_UP)
		{
			m_rotation.setX(m_rotation.getX() - 10.0f);
		}

		if (pRealEvent->m_keyId == VK_DOWN)
		{
			m_rotation.setX(m_rotation.getX() + 10.0f);
		}

		if (pRealEvent->m_keyId == VK_LEFT)
		{
			m_rotation.setY(m_rotation.getY() - 10.0f);
		}

		if (pRealEvent->m_keyId == VK_RIGHT)
		{
			m_rotation.setY(m_rotation.getY() + 10.0f);
		}
	}



}