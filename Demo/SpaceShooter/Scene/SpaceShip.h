#pragma once

#include "Scene/RenderComponent.h"

namespace ZE
{
	class Event;
	class GameContext;

	class SpaceShip : public RenderComponent
	{
		DEFINE_CLASS(SpaceShip)

	public:
		SpaceShip(GameContext* gameContext) : RenderComponent(gameContext)
		{};
		virtual ~SpaceShip() {}
		virtual void setupComponent() override;

		void handleMovementInput(Event* evt);
		virtual void handleUpdateEvent(Event* event) override;

		Vector3 m_movementSpeed;
	};
}
