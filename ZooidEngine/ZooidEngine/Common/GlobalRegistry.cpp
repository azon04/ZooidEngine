#include "GlobalRegistry.h"

#include "../ZEngineIncl.h"

#include "Scene/DebugCamera.h"

namespace ZE {
	void GlobalRegistry::Register()
	{
		// Register all object classes
		Component::RegClass();
		Actor::RegClass();
		SceneComponent::RegClass();
		CameraComponent::RegClass();
		DebugCamera::RegClass();
		CameraManager::RegClass();
		EventDispatcher::RegClass();
		InputManager::RegClass();
		KeyboardMouseInput::RegClass();

		// Register event classes
		Event::RegClass();
		Event_UPDATE::RegClass();
		Event_INPUT::RegClass();
			Event_KEY_UP::RegClass();
			Event_KEY_DOWN::RegClass();

	}

}