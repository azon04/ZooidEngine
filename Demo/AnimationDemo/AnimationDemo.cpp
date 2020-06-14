#include "AnimationDemo.h"

#include "ZEGameContext.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/ResourceManagers/AnimationManager.h"

#include "ZooidEngine/Animation/AnimationComponent.h"
#include "ZooidEngine/Animation/Animation.h"

#include "ZooidEngine/Scene/CameraManager.h"
#include "ZooidEngine/Scene/CameraComponent.h"

namespace ZE
{
	void AnimationDemo::Setup(GameContext* _gameContext)
	{
		_gameContext->getSceneManager()->loadSceneFile(GetPackageAssetPath("TestAnimation", "Scene", "Test.scz").c_str());

		{
			CameraComponent* cameraComp = CameraManager::GetInstance()->getCurrentCamera();
			Quaternion q;
			Matrix4x4 transform;
			transform.fromQuaternion(q);
			transform.setPos(Vector3(0.0f, 1.0f, 5.0f));
			cameraComp->setWorldTransform(transform);
		}
	}

	void AnimationDemo::Tick(GameContext* _gameContext, float deltaTime)
	{

	}

	void AnimationDemo::Clean(GameContext* _gameContext)
	{

	}

	Application* Application::GetApplication()
	{
		static AnimationDemo application;
		return &application;
	}

}
