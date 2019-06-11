#ifndef __ZE_SKELETON_MANAGER_H__
#define __ZE_SKELETON_MANAGER_H__

#include "ResourceManager.h"

namespace ZE
{
	class GameContext;

	class SkeletonManager : public ResourceManager
	{
		DEFINE_CLASS(SkeletonManager)

	public:
		SkeletonManager() {}

		static void Init(GameContext* _gameContext);
		static void Destroy();
		static SkeletonManager* GetInstance();

	protected:
		// ResourceManager implementation
		virtual Handle loadResource_Internal(const char* resourceFilePath, ResourceCreateSettings* settings) override;
		virtual void preUnloadResource(Resource* _resource) override;

	protected:
		GameContext* m_gameContext;

		static SkeletonManager* s_instance;
	};
}
#endif
