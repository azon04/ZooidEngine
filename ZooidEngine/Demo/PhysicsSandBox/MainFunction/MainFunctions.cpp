#include "MainFunctions.h"

#include "ZEGameContext.h"

#include "ZooidEngine/Scene/SceneManager.h"
#include "ZooidEngine/FileSystem/DirectoryHelper.h"

#include "Scene/ControlledComponent.h"

namespace PhysicsSandBox
{

	void MainSetup(ZE::GameContext* _gameContext)
	{
		_gameContext->getSceneManager()->LoadSceneFile(ZE::GetPackageAssetPath("PhysicsSandBox", "Scene", "Test.scz").c_str());
		
		{
			ZE::Handle h("", sizeof(ControlledComponent));
			ControlledComponent* controlledComponent = new(h) ControlledComponent(_gameContext);
			
			controlledComponent->setWorldPosition(Vector3(0.25f, 0.5f, 7.0f));
			controlledComponent->setupComponent();

			_gameContext->getRootComponent()->addChild(controlledComponent);
		}
	}

	void MainClean(ZE::GameContext* _gameContext)
	{

	}

}