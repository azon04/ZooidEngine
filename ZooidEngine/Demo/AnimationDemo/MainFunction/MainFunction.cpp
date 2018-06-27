#include "MainFunction.h"

#include "ZEGameContext.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/ResourceManagers/AnimationManager.h"

#include "ZooidEngine/Animation/AnimationComponent.h"
#include "ZooidEngine/Animation/Animation.h"

namespace AnimationDemo
{

	void MainSetup(ZE::GameContext* _gameContext)
	{
		_gameContext->getSceneManager()->LoadSceneFile(ZE::GetPackageAssetPath("TestAnimation", "Scene", "Test.scz").c_str());
	}

	void MainClean(ZE::GameContext* _gameContext)
	{

	}

}
