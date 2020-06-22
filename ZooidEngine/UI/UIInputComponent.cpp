#include "UIInputComponent.h"
#include "ZooidUI.h"

#include "ZEGameContext.h"
#include "Input/InputManager.h"
#include "Input/Keys.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(UIInputComponent, Component);

	UIInputComponent::UIInputComponent(GameContext* gameContext)
		: Component(gameContext)
	{

	}

	void UIInputComponent::setupComponent()
	{
		Component::setupComponent();
		addEventDelegate(Event_UPDATE, &UIInputComponent::handleUpdateEvent);
		addEventDelegate(Event_MOUSE_SCROLL, &UIInputComponent::handleMouseScroll);
		addEventDelegate(Event_KEY_UP, &UIInputComponent::handleKeyUp);
		addEventDelegate(Event_KEY_DOWN, &UIInputComponent::handleKeyDown);
		addEventDelegate(Event_TEXT_INPUT, &UIInputComponent::handleTextInput);
	}

	void UIInputComponent::handleUpdateEvent(Event* _event)
	{
		Int32 mouseX = 0;
		Int32 mouseY = 0;
		EButtonState buttonState = EButtonState::BUTTON_DOWN;

		InputManager* inputManager = m_gameContext->getInputManager();
		inputManager->getMousePosition(mouseX, mouseY);
		buttonState = inputManager->IsKeyDown(Key::MouseLeftButton) ? EButtonState::BUTTON_DOWN : EButtonState::BUTTON_UP;
		
		ZE::UI::UpdateMouseState((Float32)mouseX, (Float32)mouseY, buttonState);
	}

	void UIInputComponent::handleMouseScroll(Event* _event)
	{
		Event_MOUSE_SCROLL* pRealEvent = (Event_MOUSE_SCROLL*)_event;

		ZE::UI::RecordMouseScroll(pRealEvent->m_deltaScrollY);
	}

	void UIInputComponent::handleKeyUp(Event* _event)
	{
		Event_INPUT* inputEvent = (Event_INPUT*)_event;

		ZE::UI::RecordKeyboardButton(inputEvent->m_keyId, 1);
	}

	void UIInputComponent::handleKeyDown(Event* _event)
	{
		Event_INPUT* inputEvent = (Event_INPUT*)_event;

		ZE::UI::RecordKeyboardButton(inputEvent->m_keyId, 0);
	}

	void UIInputComponent::handleTextInput(Event* _event)
	{
		Event_TEXT_INPUT* inputEvent = (Event_TEXT_INPUT*)_event;

		ZE::UI::RecordTextInput(inputEvent->m_charCode);
	}

}