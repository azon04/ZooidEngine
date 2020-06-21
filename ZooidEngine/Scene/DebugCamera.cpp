#include "DebugCamera.h"

#include "Events/InputEvents.h"
#include "Input/InputManager.h"
#include "Input/KeyboardMouseInput.h"
#include "ZEGameContext.h"

#include "Input/Keys.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(DebugCamera, CameraComponent);

	void DebugCamera::setupComponent()
	{
		CameraComponent::setupComponent();
		//addEventDelegate(Event_KEY_UP, &DebugCamera::handleKeyUpEvent);
		//addEventDelegate(Event_KEY_DOWN, &DebugCamera::handleKeyDownEvent);
		addEventDelegate(Event_MOUSE_DRAG, &DebugCamera::handleMouseDragEvent);
	}

	void DebugCamera::handleUpdateEvent(Event* _event)
	{
		processInput();

		Event_UPDATE* pRealEvent = static_cast<Event_UPDATE*>(_event);
		Float32 deltaInSeconds = pRealEvent->m_deltaSeconds;
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

		if (pRealEvent->m_keyId == Key::UpArrow)
		{
			m_rotation.setX(m_rotation.getX() + 20.0f);
		}

		if (pRealEvent->m_keyId == Key::DownArrow)
		{
			m_rotation.setX(m_rotation.getX() - 20.0f);
		}

		if (pRealEvent->m_keyId == Key::LeftArrow)
		{
			m_rotation.setY(m_rotation.getY() + 20.0f);
		}

		if (pRealEvent->m_keyId == Key::RightArrow)
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

		if (pRealEvent->m_keyId == Key::UpArrow)
		{
			m_rotation.setX(m_rotation.getX() - 20.0f);
		}

		if (pRealEvent->m_keyId == Key::DownArrow)
		{
			m_rotation.setX(m_rotation.getX() + 20.0f);
		}

		if (pRealEvent->m_keyId == Key::LeftArrow)
		{
			m_rotation.setY(m_rotation.getY() - 20.0f);
		}

		if (pRealEvent->m_keyId == Key::RightArrow)
		{
			m_rotation.setY(m_rotation.getY() + 20.0f);
		}
	}

	void DebugCamera::handleMouseDragEvent(Event* _event)
	{
		Event_MOUSE_DRAG* pMouseDragEvent = (Event_MOUSE_DRAG*)_event;

		if (pMouseDragEvent->m_keyId == Key::MouseRightButton)
		{
			float deltaPitch = pMouseDragEvent->m_deltaY * 0.001f;
			float deltaYaw = pMouseDragEvent->m_deltaX * 0.001f;

			m_worldTransform.rotateAroundV(-deltaYaw);
			m_worldTransform.rotateAroundU(-deltaPitch);
		}
	}

	void DebugCamera::processInput()
	{
		InputManager* inputManager = m_gameContext->getInputManager();

		m_velocity.setX(0.0f);
		m_velocity.setY(0.0f);
		m_velocity.setZ(0.0f);

		if (inputManager)
		{
			if (inputManager->IsKeyDown(Key::W))
			{
				m_velocity.m_z -= 2.0f;
			}

			if (inputManager->IsKeyDown(Key::S))
			{
				m_velocity.m_z += 2.0f;
			}

			if (inputManager->IsKeyDown(Key::A))
			{
				m_velocity.m_x -= 2.0f;
			}

			if (inputManager->IsKeyDown(Key::D))
			{
				m_velocity.m_x += 2.0f;
			}
		}

		m_rotation.setX(0.0f);
		m_rotation.setY(0.0f);

		if (inputManager->IsKeyDown(Key::UpArrow))
		{
			m_rotation.setX(m_rotation.getX() + 20.0f);
		}

		if (inputManager->IsKeyDown(Key::DownArrow))
		{
			m_rotation.setX(m_rotation.getX() - 20.0f);
		}

		if (inputManager->IsKeyDown(Key::LeftArrow))
		{
			m_rotation.setY(m_rotation.getY() + 20.0f);
		}

		if (inputManager->IsKeyDown(Key::RightArrow))
		{
			m_rotation.setY(m_rotation.getY() - 20.0f);
		}
	}

}