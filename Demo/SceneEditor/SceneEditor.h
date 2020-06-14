#pragma once

#include "Application/Application.h"

namespace ZE
{
	class GameContext;

	class SceneEditor : public Application
	{
	public:
		virtual void Setup(GameContext* _gameContext);
		virtual void Tick(GameContext* _gameContext, float deltaTime);
		virtual void Clean(GameContext* _gameContext);
	};
}