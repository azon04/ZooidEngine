#ifndef __ZE_ANIMATION_MANAGER_H__
#define __ZE_ANIMATION_MANAGER_H__

#include "ResourceManager.h"

namespace ZE
{
	class GameContext;

	class AnimationManager : public ResourceManager
	{

		DEFINE_CLASS(AnimationManager)
	public:

		static void Init(GameContext* gameContext);
		static void Destroy();
		static AnimationManager* getInstance() { return s_instance; }

		virtual Handle loadResource_Internal(const char* resourceFilePath) override;
		virtual void preUnloadResource(Resource* _resource) override;

	protected:

		static AnimationManager* s_instance;
	};
}
#endif
