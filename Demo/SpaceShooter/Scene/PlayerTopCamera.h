#pragma once

#include "ZooidEngine/Scene/DebugCamera.h"

namespace ZE
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
