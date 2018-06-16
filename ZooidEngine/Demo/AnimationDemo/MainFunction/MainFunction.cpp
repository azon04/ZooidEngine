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
		
		ZE::Handle hAnimation = ZE::AnimationManager::getInstance()->loadResource(ZE::GetPackageAssetPath("TestAnimation", "Animation", "Walking_baked_Take001.animz").c_str());
		ZE::AnimationClip* pAnimClip = hAnimation.getObject<ZE::AnimationClip>();

		ZE::Handle hSkin1 = _gameContext->getSceneManager()->getCompByName("Skin1");
		ZE::Handle hSkin2 = _gameContext->getSceneManager()->getCompByName("Skin2");

		{
			ZE::Component* pSkin1 = hSkin1.getObject<ZE::Component>();
			ZE::AnimationComponent* pAnimComp1 = pSkin1->findFirstCompByClassId<ZE::AnimationComponent>(ZE::AnimationComponent::GetClassID());
			pAnimComp1->playAnimationClip(pAnimClip, true, -1.0f);
		}

		{
			ZE::Component* pSkin2 = hSkin2.getObject<ZE::Component>();
			ZE::AnimationComponent* pAnimComp2 = pSkin2->findFirstCompByClassId<ZE::AnimationComponent>(ZE::AnimationComponent::GetClassID());
			pAnimComp2->playAnimationClip(pAnimClip, true, -1.0f);
		}
	}

	void MainClean(ZE::GameContext* _gameContext)
	{

	}

}
