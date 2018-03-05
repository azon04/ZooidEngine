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
		addEventDelegate(Event_MOUSE_DRAG, &DebugCamera::handleMouseDragEvent);
	}

	void DebugCamera::handleUpdateEvent(Event* _event)
	{
		Event_UPDATE* pRealEvent = static_cast<Event_UPDATE*>(_event);
		Float32 deltaInSeconds = (Float32)(pRealEvent->m_deltaTime / 1000.0);
		m_worldTransform.setPos(m_worldTransform.getPos() + (m_worldTransform.getU() * m_velocity.getX() + m_worldTransform.getN() * m_velocity.getZ()) * deltaInSeconds );
		m_worldTransform.rotateAroundU(DegToRad(m_rotation.getX() * deltaInSeconds));
		m_worldTransform.rotateAroundV(DegToRad(m_rotation.getY() * deltaInSeconds));
	}

	void DebugCamera::handleKeyDownEvent(Event* _event)
	{
		Event_INPUT* pRealEvent = (Event_INPUT*)_event;
		if (pRealEvent->m_keyId == 'W')
		{
			m_velocity.setZ(m_velocity.getZ() - 2.0f);
		}
		
		if (pRealEvent->m_keyId == 'S')
		{
			m_velocity.setZ(m_velocity.getZ() + 2.0f);
		}

		if (pRealEvent->m_keyId == 'A')
		{
			m_velocity.setX(m_velocity.getX() - 2.0f);
		}

		if (pRealEvent->m_keyId == 'D')
		{
			m_velocity.setX(m_velocity.getX() + 2.0f);
		}

		if (pRealEvent->m_keyId == VK_UP)
		{
			m_rotation.setX(m_rotation.getX() + 20.0f);
		}

		if (pRealEvent->m_keyId == VK_DOWN)
		{
			m_rotation.setX(m_rotation.getX() - 20.0f);
		}

		if (pRealEvent->m_keyId == VK_LEFT)
		{
			m_rotation.setY(m_rotation.getY() + 20.0f);
		}

		if (pRealEvent->m_keyId == VK_RIGHT)
		{
			m_rotation.setY(m_rotation.getY() - 20.0f);
		}
	}

	void DebugCamera::handleKeyUpEvent(Event* _event)
	{
		Event_INPUT* pRealEvent = (Event_INPUT*)_event;
		if (pRealEvent->m_keyId == 'W')
		{
			m_velocity.setZ(m_velocity.getZ() + 2.0f);
		}

		if (pRealEvent->m_keyId == 'S')
		{
			m_velocity.setZ(m_velocity.getZ() - 2.0f);
		}

		if (pRealEvent->m_keyId == 'A')
		{
			m_velocity.setX(m_velocity.getX() + 2.0f);
		}

		if (pRealEvent->m_keyId == 'D')
		{
			m_velocity.setX(m_velocity.getX() - 2.0f);
		}

		if (pRealEvent->m_keyId == VK_UP)
		{
			m_rotation.setX(m_rotation.getX() - 20.0f);
		}

		if (pRealEvent->m_keyId == VK_DOWN)
		{
			m_rotation.setX(m_rotation.getX() + 20.0f);
		}

		if (pRealEvent->m_keyId == VK_LEFT)
		{
			m_rotation.setY(m_rotation.getY() - 20.0f);
		}

		if (pRealEvent->m_keyId == VK_RIGHT)
		{
			m_rotation.setY(m_rotation.getY() + 20.0f);
		}
	}

	void DebugCamera::handleMouseDragEvent(Event* _event)
	{
		Event_MOUSE_DRAG* pMouseDragEvent = (Event_MOUSE_DRAG*)_event;

		if (pMouseDragEvent->m_keyId == VK_RBUTTON)
		{
			float deltaPitch = pMouseDragEvent->m_deltaY * 0.001;
			float deltaYaw = pMouseDragEvent->m_deltaX * 0.001;

			m_worldTransform.rotateAroundV(deltaYaw);
			m_worldTransform.rotateAroundU(deltaPitch);
		}
	}

}