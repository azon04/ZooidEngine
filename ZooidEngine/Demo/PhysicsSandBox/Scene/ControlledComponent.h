#pragma once

#include "Scene/RenderComponent.h"

namespace PhysicsSandBox
{
	class ControlledComponent : public ZE::RenderComponent
	{
		DEFINE_CLASS(ControlledComponent);

	public:

		ControlledComponent(ZE::GameContext* gameContext) : RenderComponent(gameContext)
		{}

		virtual void setupComponent() override;

		void handleKeyUpEvent(ZE::Event* event);
		void handleKeyDownEvent(ZE::Event* event);
	};
}
