#include "GlobalRegistry.h"

#include "../ZEngineIncl.h"

#include "Scene/DebugCamera.h"
#include "Scene/Light/LightComponent.h"

// Resource Managers
#include "ResourceManagers/MeshManager.h"
#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/TextureManager.h"

#include "Resources/Material.h"
#include "Resources/Mesh.h"
#include "Resources/Texture.h"

#include "Physics/PhysicsEvents.h"

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
		LightComponent::RegClass();

		// Register Resource Classes
		Material::RegClass();
		Mesh::RegClass();
		Texture::RegClass();

		ResourceManager::RegClass();
		MeshManager::RegClass();
		ShaderManager::RegClass();
		TextureManager::RegClass();

		// Register event classes
		Event::RegClass();
		Event_UPDATE::RegClass();
		Event_INPUT::RegClass();
			Event_KEY_UP::RegClass();
			Event_KEY_DOWN::RegClass();
			Event_MOUSE_DRAG::RegClass();
		Event_GATHER_LIGHT::RegClass();
		Event_GATHER_RENDER::RegClass();
		Event_RESOURCE_LOADED::RegClass();

		Event_Physics_BASE::RegClass();
		Event_Physics_ONCOLLIDE::RegClass();
		Event_Physics_PREUPDATE::RegClass();
		Event_Physics_UPDATE::RegClass();
		Event_Physics_POSTUPDATE::RegClass();
		Event_Physics_UPDATE_TRANSFORM::RegClass();
		Event_Physics_ON_TRIGGER::RegClass();
		Event_Physics_ON_BEGIN_TRIGGER::RegClass();
		Event_Physics_ON_END_TRIGGER::RegClass();
	}

}