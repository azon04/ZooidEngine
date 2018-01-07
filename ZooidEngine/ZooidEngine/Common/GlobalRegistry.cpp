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
		Event_GATHER_LIGHT::RegClass();
		Event_RESOURCE_LOADED::RegClass();
	}

}