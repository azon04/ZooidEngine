#include "SpaceShooter.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"
#include "ZooidEngine/Memory/Handle.h"
#include "ZooidEngine/Scene/RenderComponent.h"
#include "ZooidEngine/Scene/CameraManager.h"

#include "Scene/SpaceShip.h"
#include "Scene/PlayerTopCamera.h"
#include "Scene/MeteorManager.h"

#include "ZEGameContext.h"

namespace ZE
{
	void SpaceShooter::Setup(GameContext* _gameContext)
	{
		_gameContext->getSceneManager()->loadSceneFile(ZE::GetResourcePath("SpaceShooter/Scene/Test.scz").c_str());

		{
			ZE::Handle h("SpaceShip", sizeof(SpaceShip));
			SpaceShip* spaceShip = new(h) SpaceShip(_gameContext);
			spaceShip->setupComponent();
			_gameContext->getRootComponent()->addChild(spaceShip);
		}

		{
			ZE::Handle h("MeteorManager", sizeof(MeteorManager));
			MeteorManager* pMeteorManager = new(h) MeteorManager(_gameContext);
			pMeteorManager->setupComponent();
			_gameContext->getRootComponent()->addChild(pMeteorManager);
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

	void SpaceShooter::Tick(GameContext* _gameContext, float deltaTime)
	{

	}

	void SpaceShooter::Clean(GameContext* _gameContext)
	{

	}

	Application* Application::GetApplication()
	{
		static SpaceShooter application;
		return &application;
	}
}

