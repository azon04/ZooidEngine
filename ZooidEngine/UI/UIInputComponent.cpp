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

}