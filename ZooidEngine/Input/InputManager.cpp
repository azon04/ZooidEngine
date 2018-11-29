#include "InputManager.h"

#include "KeyboardMouseInput.h"

namespace ZE
{

	IMPLEMENT_CLASS_1(InputManager, Component);

	ZE::KeyboardMouseInput* InputManager::getKeyboardMouseInput()
	{
		return m_keyboardMouseInput;
	}

	void InputManager::setupComponent()
	{
		Handle handle("KeyboardMouseInput", sizeof(KeyboardMouseInput));
		m_keyboardMouseInput = new(handle) KeyboardMouseInput(m_gameContext);
		m_keyboardMouseInput->setupComponent();
		addChild(m_keyboardMouseInput);
	}

	bool InputManager::IsKeyDown(Short m_keyId)
	{
		return getKeyboardMouseInput()->IsKeyDown(m_keyId);
	}

	void InputManager::getMousePosition(Int32& mouseX, Int32& mouseY)
	{
		if (m_keyboardMouseInput)
		{
			m_keyboardMouseInput->getMousePosition(mouseX, mouseY);
		}
	}

}