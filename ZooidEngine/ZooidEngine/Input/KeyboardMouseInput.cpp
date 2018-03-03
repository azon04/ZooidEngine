#include "KeyboardMouseInput.h"

#include "ZEGameContext.h"
#include "Events/InputEvents.h"
#include "Events/EventDispatcher.h"
#include "Memory/MemoryHelper.h"

#if defined(_WIN32) || defined(_WIN64) 
#include <Windows.h>
#else
#endif

namespace ZE {

	IMPLEMENT_CLASS_1(KeyboardMouseInput, Component)

	void KeyboardMouseInput::setupComponent()
	{
		addEventDelegate(Event_UPDATE, &KeyboardMouseInput::handleUpdate);
		GetKeyboardState(m_keyStates);
		GetKeyboardState(m_prevState);

		m_isCurrentDragged = false;
	}

	void KeyboardMouseInput::handleUpdate(Event* event)
	{
		Event_UPDATE* pEventUpdate = (Event_UPDATE*)event;

		POINT p;
		if (GetCursorPos(&p))
		{
			ScreenToClient(m_gameContext->getRenderer()->getWinWindow(), &p);
			m_mouseX = p.x;
			m_mouseY = p.y;
		}
		
		if (GetKeyboardState(m_keyStates))
		{
			for (Short i = 0; i < 256; i++)
			{
				if (m_keyStates[i] ^ m_prevState[i])
				{
					if (m_keyStates[i] & 0x80)
					{
						Handle handle("InputEvent", sizeof(Event_KEY_DOWN));
						Event_KEY_DOWN* pEvent = new(handle) Event_KEY_DOWN;
						pEvent->m_keyId = i;
						m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);	

						if (i == VK_LBUTTON && !m_isCurrentDragged)
						{
							m_currentDragTime = 0.0f;
							m_isCurrentDragged = true;
							m_startDragX = m_mouseX;
							m_startDragY = m_mouseY;
						}
					}
					else
					{
						Handle handle("InputEvent", sizeof(Event_KEY_UP));
						Event_KEY_UP* pEvent = new(handle) Event_KEY_UP;
						pEvent->m_keyId = i;
						m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);

						if (i == VK_LBUTTON && m_isCurrentDragged)
						{
							m_isCurrentDragged = false;
						}
					}
					m_prevState[i] = m_keyStates[i];
				}
			}
		}

		if (m_isCurrentDragged)
		{
			m_currentDragTime += pEventUpdate->m_deltaTime;

			const float deltaTime = 500.0f;
			if (m_currentDragTime > deltaTime)
			{
				Handle handle("MouseDragInput", sizeof(Event_MOUSE_DRAG));
				Event_MOUSE_DRAG* pEvent = new(handle) Event_MOUSE_DRAG;
				pEvent->m_keyId = VK_LBUTTON;
				pEvent->m_deltaX = m_mouseX - m_startDragX;
				pEvent->m_deltaY = m_mouseY - m_startDragY;

				m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);

				m_startDragX = m_mouseX;
				m_startDragY = m_mouseY;
				m_currentDragTime -= deltaTime;
			}
		}

	}

}
