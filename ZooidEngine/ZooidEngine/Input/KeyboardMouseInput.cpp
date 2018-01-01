#include "KeyboardMouseInput.h"

#include "ZEGameContext.h"
#include "Events/InputEvents.h"
#include "Events/EventDispatcher.h"
#include "MemoryManagement/MemoryHelper.h"

#include <Windows.h>

namespace ZE {

	IMPLEMENT_CLASS_1(KeyboardMouseInput, Component)

	void KeyboardMouseInput::setupComponent()
	{
		addEventDelegate(Event_UPDATE, &KeyboardMouseInput::handleUpdate);
		GetKeyboardState(m_keyStates);
		GetKeyboardState(m_prevState);
	}


	void KeyboardMouseInput::handleUpdate(Event* event)
	{
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
					}
					else
					{
						Handle handle("InputEvent", sizeof(Event_KEY_UP));
						Event_KEY_UP* pEvent = new(handle) Event_KEY_UP;
						pEvent->m_keyId = i;
						m_gameContext->getEventDispatcher()->addEvent(handle, EVENT_INPUT);
					}
					m_prevState[i] = m_keyStates[i];
				}
			}
		}

	}

}
