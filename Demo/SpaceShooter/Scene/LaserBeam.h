#pragma once

#include "Scene/RenderComponent.h"

class ZE::Event;
class ZE::GameContext;

namespace ZE
{
	class LaserBeam : public ZE::RenderComponent
	{
		DEFINE_CLASS(LaserBeam)

	public:
		LaserBeam(ZE::GameContext* _gameContex, Vector3 velocity, Vector3 initialPosition);
		virtual ~LaserBeam() {}
		virtual void setupComponent() override;

		virtual void handleUpdateEvent(ZE::Event* event) override;

		Vector3 m_velocity;
	};
}