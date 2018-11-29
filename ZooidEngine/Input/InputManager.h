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

		// get keyboard mouse input component
		KeyboardMouseInput* getKeyboardMouseInput();

		virtual void setupComponent() override;

		bool IsKeyDown(Short m_keyId);
		void getMousePosition(Int32& mouseX, Int32& mouseY);

	protected:
		// Cache KeyboardMouseInput
		KeyboardMouseInput* m_keyboardMouseInput;
	};
}
#endif
