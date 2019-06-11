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
		static AnimationManager* GetInstance() { return s_instance; }

	protected:
		// Resource Manager implementation
		virtual Handle loadResource_Internal(const char* resourceFilePath, ResourceCreateSettings* settings) override;
		virtual void preUnloadResource(Resource* _resource) override;

	private:
		static AnimationManager* s_instance;
	};
}
#endif
