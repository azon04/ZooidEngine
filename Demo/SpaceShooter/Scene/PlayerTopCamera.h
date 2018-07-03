#pragma once

#include "ZooidEngine/Scene/DebugCamera.h"

namespace SpaceShooter
{
	class PlayerTopCamera : public ZE::CameraComponent
	{
		DEFINE_CLASS(PlayerTopCamera)

	public:

		PlayerTopCamera(ZE::GameContext* _gameContext) : ZE::CameraComponent(_gameContext)
		{}
		
		virtual void setupComponent() override;
	};
}
