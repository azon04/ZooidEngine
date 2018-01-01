#include "InputManager.h"

#include "KeyboardMouseInput.h"

namespace ZE
{

	IMPLEMENT_CLASS_1(InputManager, Component);

	ZE::KeyboardMouseInput* InputManager::getKeyboardMouseInput()
	{
		for (int i = 0; i < m_components.length(); i++)
		{
			if (m_components[i]->getClassID() == KeyboardMouseInput::GetClassID())
			{
				return (KeyboardMouseInput*)m_components[i];
			}
		}

		return nullptr;
	}

	void InputManager::setupComponent()
	{
		{
			Handle handle("KeyboardMouseInput", sizeof(KeyboardMouseInput));
			KeyboardMouseInput* pKMInput = new(handle) KeyboardMouseInput(m_gameContext);
			pKMInput->setupComponent();
			addChild(pKMInput);
		}
	}

	bool InputManager::IsKeyDown(Short m_keyId)
	{
		return getKeyboardMouseInput()->IsKeyDown(m_keyId);
	}

}