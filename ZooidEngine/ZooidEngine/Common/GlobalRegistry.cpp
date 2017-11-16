#include "GlobalRegistry.h"

#include "../ZEngineIncl.h"

namespace ZE {
	void GlobalRegistry::Register()
	{
		// Register all object classes
		Component::RegisterClass();
		Actor::RegisterClass();
		SceneComponent::RegisterClass();
		CameraComponent::RegisterClass();
		CameraManager::RegisterClass();
		EventDispatcher::RegisterClass();

		// Register event classes
		Event::RegisterClass();
		Event_UPDATE::RegisterClass();
	}

}