#pragma once

#include "ZooidEngine/Scene/RenderComponent.h"

namespace ZE
{
	class Meteor : public ZE::RenderComponent
	{
		DEFINE_CLASS(Meteor)

	public:
		Meteor(ZE::GameContext* _gameContext, Vector3 velocity, Vector3 initialPosition);
		virtual ~Meteor() {}

		virtual void setupComponent() override;

		virtual void handleUpdateEvent(ZE::Event* event) override;

		Vector3 m_velocity;
	};
}