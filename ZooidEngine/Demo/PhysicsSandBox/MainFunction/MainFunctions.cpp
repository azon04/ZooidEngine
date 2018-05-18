#include "MainFunctions.h"

#include "ZEGameContext.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"

namespace PhysicsSandBox
{

	void MainSetup(ZE::GameContext* _gameContext)
	{
		_gameContext->getSceneManager()->LoadSceneFile(ZE::GetPackageAssetPath("PhysicsSandBox", "Scene", "Test.scz").c_str());
	}

	void MainClean(ZE::GameContext* _gameContext)
	{

	}

}