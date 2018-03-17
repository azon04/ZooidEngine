#include "MainFunctions.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"

void SpaceShooter::MainSetup(ZE::GameContext* _gameContext)
{
	ZE::SceneManager::GetInstance()->LoadSceneFile(ZE::GetResourcePath("Basic/Scene/Test.scz").c_str());
}

void SpaceShooter::MainClean(ZE::GameContext* _gameContext)
{

}

