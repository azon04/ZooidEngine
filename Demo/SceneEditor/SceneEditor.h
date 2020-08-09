#pragma once

#include "Application/Application.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	class GameContext;
	class SceneComponent;

	class SceneEditor : public Application
	{
	public:
		virtual void Setup(GameContext* _gameContext);
		virtual void Tick(GameContext* _gameContext, float deltaTime);
		virtual void Clean(GameContext* _gameContext);

	protected:
		Int32 m_selectedIndex = -1;
		SceneComponent* m_selectedRenderComponent = nullptr;
	};
}