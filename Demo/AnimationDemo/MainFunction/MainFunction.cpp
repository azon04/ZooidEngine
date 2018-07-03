#include "MainFunction.h"

#include "ZEGameContext.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/ResourceManagers/AnimationManager.h"

#include "ZooidEngine/Animation/AnimationComponent.h"
#include "ZooidEngine/Animation/Animation.h"

#include "ZooidEngine/Scene/CameraManager.h"
#include "ZooidEngine/Scene/CameraComponent.h"

namespace AnimationDemo
{

	void MainSetup(ZE::GameContext* _gameContext)
	{
		_gameContext->getSceneManager()->loadSceneFile(ZE::GetPackageAssetPath("TestAnimation", "Scene", "Test.scz").c_str());
		
		{
			ZE::CameraComponent* cameraComp = ZE::CameraManager::GetInstance()->getCurrentCamera();
			Quaternion q;
			Matrix4x4 transform;
			transform.fromQuaternion(q);
			transform.setPos(Vector3(0.0f, 1.0f, 5.0f));
			cameraComp->setWorldTransform(transform);
		}
	}

	void MainClean(ZE::GameContext* _gameContext)
	{

	}

}
