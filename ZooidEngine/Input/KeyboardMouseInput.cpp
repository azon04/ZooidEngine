#include "KeyboardMouseInput.h"

#include "ZEGameContext.h"
#include "Events/InputEvents.h"
#include "Events/EventDispatcher.h"
#include "Memory/MemoryHelper.h"
#include "Renderer/IRenderer.h"

namespace ZE 
{

	IMPLEMENT_CLASS_1(KeyboardMouseInput, Component)

	void KeyboardMouseInput::setupComponent()
	{
		addEventDelegate(Event_UPDATE, &KeyboardMouseInput::handleUpdate);

		m_isCurrentDragged = false;
	}

	void KeyboardMouseInput::handleUpdate(Event* event)
	{
		Event_UPDATE* pEventUpdate = (Event_UPDATE*)event;

		static const Short dragKeys[] = { Key::MouseLeftButton, Key::MouseRightButton };

		for (int i = 0; i < 2; ++i)
		{
			const bool mouseHold = IsKeyDown(dragKeys[i]);

			if (mouseHold && (!m_isCurrentDragged || m_currentDragKey != dragKeys[i]))
			{
				m_currentDragTime = 0.0f;
				m_isCurrentDragged = true;
				m_startDragX = m_mouseX;
				m_startDragY = m_mouseY;
				m_currentDragKey = dragKeys[i];
			}
			else if (!mouseHold && m_isCurrentDragged && m_currentDragKey == dragKeys[i])
			{
				m_isCurrentDragged = false;
			}
		}

		if (m_isCurrentDragged)
		{
			m_currentDragTime += pEventUpdate->m_deltaSeconds;

			const float deltaTime = 0.01f;
			if (m_currentDragTime > deltaTime)
			{
				Handle handle("MouseDragInput", sizeof(Event_MOUSE_DRAG));
				Event_MOUSE_DRAG* pEvent = new(handle) Event_MOUSE_DRAG;
				pEvent->m_keyId = m_currentDragKey;
				pEvent->m_deltaX = m_mouseX - m_startDragX;
				pEvent->m_deltaY = m_mouseY - m_startDragY;

				m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);

				m_startDragX = m_mouseX;
				m_startDragY = m_mouseY;
				m_currentDragTime -= deltaTime;
			}
		}

	}

	void KeyboardMouseInput::setKey(Short keyId, KeyState::KeyStateEnum keyState)
	{
		if (keyState == KeyState::Pressed)
		{
			const bool bTriggerEvent = !IsKeyDown(keyId);
			m_keyStates[keyId] = 0xFF;

			if (bTriggerEvent)
			{
				Handle handle("InputEvent", sizeof(Event_KEY_DOWN));
				Event_KEY_DOWN* pEvent = new(handle) Event_KEY_DOWN;
				pEvent->m_keyId = keyId;
				m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);
			}
		}
		else if( keyState == KeyState::Released )
		{
			const bool bTriggerEvent = IsKeyDown(keyId);
			m_keyStates[keyId] = 0x00;

			if (bTriggerEvent)
			{
				Handle handle("InputEvent", sizeof(Event_KEY_UP));
				Event_KEY_UP* pEvent = new(handle) Event_KEY_UP;
				pEvent->m_keyId = keyId;
				m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);
			}
		}
	}

	void KeyboardMouseInput::setMousePosition(int mouseX, int mouseY)
	{
		m_mouseX = mouseX;
		m_mouseY = mouseY;
	}

	void KeyboardMouseInput::setMouseScroll(float mouseScrollX, float mouseScrollY)
	{
		{
			Handle handle("InputEvent", sizeof(Event_MOUSE_SCROLL));
			Event_MOUSE_SCROLL* pEvent = new(handle) Event_MOUSE_SCROLL;
			pEvent->m_deltaScrollX = mouseScrollX;
			pEvent->m_deltaScrollY = mouseScrollY;
			m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);
		}

		m_lastMouseScrollX = mouseScrollX;
		m_lastMouseScrollY = mouseScrollY;
	}

	void KeyboardMouseInput::handleTextInput(UInt32 charCode)
	{
		Handle handle("InputEvent", sizeof(Event_TEXT_INPUT));
		Event_TEXT_INPUT* pEvent = new(handle) Event_TEXT_INPUT;
		pEvent->m_charCode = charCode;
		m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);
	}

}
