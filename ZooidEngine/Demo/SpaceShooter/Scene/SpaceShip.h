#pragma once

#include "Scene/RenderComponent.h"

class ZE::Event;
class ZE::GameContext;

namespace SpaceShooter
{
	class SpaceShip : public ZE::RenderComponent
	{
		DEFINE_CLASS(SpaceShip)

	public:
		SpaceShip(ZE::GameContext* gameContext) : ZE::RenderComponent(gameContext)
		{};
		virtual ~SpaceShip() {}
		virtual void setupComponent() override;

		void handleMovementInput(ZE::Event* evt);
		virtual void handleUpdateEvent(ZE::Event* event) override;

		Vector3 m_movementSpeed;
	};
}
