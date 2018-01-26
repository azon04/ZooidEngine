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
		static TextureManager* getInstance();
		static void Destroy();

		virtual Handle loadResource_Internal(const char* resourceFilePath);
		virtual void preUnloadResource(Resource* _resource);

		GameContext* m_gameContext;
	protected:

		static TextureManager* s_instance;
	};
};
#endif
