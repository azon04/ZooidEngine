#include "UIManager.h"
#include "UIInputComponent.h"

#include "ZEGameContext.h"
#include "Renderer/IRenderer.h"
#include "Memory/Handle.h"
#include "Scene/SceneComponent.h"

namespace ZE
{

	UIManager::UIManager()
	{

	}

	UIManager::~UIManager()
	{

	}

	void UIManager::Init(GameContext* _gameContext)
	{
		ZE::UI::Init(_gameContext->getRenderer()->GetWidth(), _gameContext->getRenderer()->GetHeight());
		
		Handle hInputComponent("Input Component", sizeof(UIInputComponent));
		UIInputComponent* inputComponent = new(hInputComponent) UIInputComponent(_gameContext);
		inputComponent->setupComponent();
		_gameContext->getRootComponent()->addChild(inputComponent);

		Handle hUIManager("UIManager", sizeof(UIManager));
		s_instance = new(hUIManager) UIManager();
	}

	void UIManager::Destroy()
	{
		ZE::UI::Destroy();
	}

	ZE::UIManager* UIManager::s_instance = nullptr;

}