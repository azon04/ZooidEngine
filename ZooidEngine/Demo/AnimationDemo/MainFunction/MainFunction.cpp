#include "MainFunction.h"

#include "ZEGameContext.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/ResourceManagers/SkeletonManager.h"

namespace AnimationDemo
{

	void MainSetup(ZE::GameContext* _gameContext)
	{
		_gameContext->getSceneManager()->LoadSceneFile(ZE::GetPackageAssetPath("TestAnimation", "Scene", "Test.scz").c_str());
		
		ZE::SkeletonManager::getInstance()->loadResource(ZE::GetPackageAssetPath("TestAnimation", "Skeleton", "Walking.fbx.skelz").c_str());
	}

	void MainClean(ZE::GameContext* _gameContext)
	{

	}

}
