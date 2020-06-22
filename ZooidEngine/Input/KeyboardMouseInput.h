#ifndef __ZE_KEYBOARD_MOUSE_INPUT_H__
#define __ZE_KEYBOARD_MOUSE_INPUT_H__

#include "GameObjectModel/Component.h"
#include "Utils/PrimitiveTypes.h"
#include "Keys.h"

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

		// handle event update
		void handleUpdate(Event* event);

		// Get current mouse position relative to the window
		FORCEINLINE void getMousePosition(int& mouseX, int& mouseY)
		{
			mouseX = m_mouseX;
			mouseY = m_mouseY;
		}

		FORCEINLINE bool IsKeyDown(Short keyId) const
		{
			return ( m_keyStates[keyId] & 0x80 ) > 0;
		}

		void setKey(Short keyId, KeyState::KeyStateEnum keyState);
		void setMousePosition(int mouseX, int mouseY);
		void setMouseScroll(float mouseScrollX, float mouseScrollY);

		// Handling text input. charCode: character code of what user input
		void handleTextInput(UInt32 charCode);

	protected:

		Byte m_keyStates[256] = { 0 };

		int m_mouseX;
		int m_mouseY;

		// For dragging
		int m_startDragX;
		int m_startDragY;
		float m_currentDragTime;
		bool m_isCurrentDragged;
		Short m_currentDragKey;

		// For Mouse scrolling
		float m_lastMouseScrollX;
		float m_lastMouseScrollY;
	};
};

#endif