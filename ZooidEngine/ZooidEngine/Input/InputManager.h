#ifndef __ZE_INPUT_MANAGER_H__
#define __ZE_INPUT_MANAGER_H__

#include "GameObjectModel/Component.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	class KeyboardMouseInput;

	enum CONTROLLER_INPUT_ABSTRACTION : Byte
	{
		
	};

	class InputManager : public Component
	{
		DEFINE_CLASS(InputManager)

	public:

		InputManager(GameContext* inGameContext) 
			: Component(inGameContext)
		{}

		KeyboardMouseInput* getKeyboardMouseInput();

		virtual void setupComponent();

		bool IsKeyDown(Short m_keyId);
	};
}
#endif
