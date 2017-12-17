#include "KeyboardMouseInput.h"

#include "Events/Events.h"
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
			for (int i = 0; i < 256; i++)
			{
				if (m_keyStates[i] ^ m_prevState[i])
				{
					m_prevState[i] = m_keyStates[i];
				}
			}
		}

	}

}
