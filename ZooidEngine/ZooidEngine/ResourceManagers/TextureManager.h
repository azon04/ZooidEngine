#ifndef __ZE_TEXTURE_MANAGER_H__
#define __ZE_TEXTURE_MANAGER_H__

#include "ResourceManager.h"

namespace ZE 
{
	class GameContext;

	class TextureManager : public ResourceManager
	{
		DEFINE_CLASS(TextureManager)

	public:
		static void Init(GameContext* _gameContext);
		static TextureManager* GetInstance();
		static void Destroy();

	protected:
		// ResourceManager Implementation
		virtual Handle loadResource_Internal(const char* resourceFilePath);
		virtual void preUnloadResource(Resource* _resource);

	protected:
		GameContext* m_gameContext;

		static TextureManager* s_instance;
	};
};
#endif
