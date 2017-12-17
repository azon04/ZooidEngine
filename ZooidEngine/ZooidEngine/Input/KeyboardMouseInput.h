#ifndef __ZE_KEYBOARD_MOUSE_INPUT_H__
#define __ZE_KEYBOARD_MOUSE_INPUT_H__

#include "GameObjectModel/Component.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	class Event;

	class KeyboardMouseInput : public Component
	{
		DEFINE_CLASS(KeyboardMouseInput)

	public:
		KeyboardMouseInput(GameContext* gameContext) : Component(gameContext) {}
		virtual ~KeyboardMouseInput() {}

		virtual void setupComponent();

		void handleUpdate(Event* event);

		FORCEINLINE bool IsKeyDown(Short keyId) const
		{
			return ( m_keyStates[keyId] & 0x80 ) > 0;
		}

	protected:

		Byte m_keyStates[256];
		Byte m_prevState[256];
	};
};

#endif