#ifndef __ZE_FONT_MANAGER_H__
#define __ZE_FONT_MANAGER_H__

#include "ResourceManager.h"

namespace ZE
{
	class GameContext;

	class FontManager : public ResourceManager
	{
		DEFINE_CLASS(FontManager)

	public:
		static void Init(GameContext* _gameContext);
		static FontManager* GetInstance() { return s_instance; }
		static void Destroy();

	protected:
		// ResourceManager Implementation
		virtual Handle loadResource_Internal(const char* resourceFilePath);
		virtual void preUnloadResource(Resource* _resource);

	protected:
		GameContext* m_gameContext;

		static FontManager* s_instance;
	};
}
#endif
