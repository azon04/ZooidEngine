#include "MainFunctions.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/Memory/Handle.h"
#include "ZooidEngine/Scene/RenderComponent.h"
#include "ZooidEngine/Scene/CameraManager.h"

#include "Scene/SpaceShip.h"
#include "Scene/PlayerTopCamera.h"

using namespace ZE;

void SpaceShooter::MainSetup(ZE::GameContext* _gameContext)
{
	_gameContext->getSceneManager()->LoadSceneFile(ZE::GetResourcePath("SpaceShooter/Scene/Test.scz").c_str());
	
	{
		ZE::Handle h("SpaceShip", sizeof(SpaceShip));
		SpaceShip* spaceShip = new(h) SpaceShip(_gameContext);
		spaceShip->setupComponent();
		_gameContext->getRootComponent()->addChild(spaceShip);
	}

	{
		ZE::Handle h("PlayerCamera", sizeof(PlayerTopCamera));
		PlayerTopCamera* cam = new(h) PlayerTopCamera(_gameContext);
		cam->setupComponent();

		_gameContext->getRootComponent()->addChild(cam);
		_gameContext->getCameraManager()->m_cameras.push_back(h);
		_gameContext->getCameraManager()->m_currentCamera = cam;

	}
}

void SpaceShooter::MainClean(ZE::GameContext* _gameContext)
{

}

