#include "SkeletonManager.h"

#include "Animation/Skeleton.h"

namespace ZE
{

	IMPLEMENT_CLASS_1(SkeletonManager, ResourceManager)

	void SkeletonManager::Init(GameContext* _gameContext)
	{
		Handle handle("Skeleton Manager", sizeof(SkeletonManager));
		s_instance = new(handle) SkeletonManager;
		s_instance->m_gameContext = _gameContext;
	}

	void SkeletonManager::Destroy()
	{
		if (s_instance)
		{
			s_instance->unloadResources();
		}
	}

	ZE::SkeletonManager* SkeletonManager::GetInstance()
	{
		return s_instance;
	}

	ZE::Handle SkeletonManager::loadResource_Internal(const char* resourceFilePath, ResourceCreateSettings* settings)
	{
		Handle hSkel("Skeleton", sizeof(Skeleton));
		Skeleton* pSkel = new(hSkel) Skeleton;
		pSkel->loadFromFile(resourceFilePath);

		return hSkel;
	}

	void SkeletonManager::preUnloadResource(Resource* _resource)
	{

	}

	ZE::SkeletonManager* SkeletonManager::s_instance;

}