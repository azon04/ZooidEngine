#include "GlobalRegistry.h"

#include "../ZEngineIncl.h"

#include "Scene/DebugCamera.h"
#include "Scene/Light/LightComponent.h"
#include "Scene/Skybox.h"
#include "Scene/RenderComponent.h"

// Resource Managers
#include "ResourceManagers/MeshManager.h"
#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/TextureManager.h"

#include "Resources/Material.h"
#include "Resources/Mesh.h"
#include "Resources/Texture.h"

#include "Physics/PhysicsEvents.h"

#include "Animation/AnimationComponent.h"

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
		AnimationComponent::RegClass();
		Skybox::RegClass();
		RenderComponent::RegClass();

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
			Event_MOUSE_SCROLL::RegClass();
			Event_TEXT_INPUT::RegClass();
		Event_GATHER_LIGHT::RegClass();
		Event_GATHER_RENDER::RegClass();
		Event_GATHER_BOUND::RegClass();
		Event_CALC_TRANSFORM::RegClass();
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