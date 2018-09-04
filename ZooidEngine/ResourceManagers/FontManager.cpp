#include "FontManager.h"
#include "Resources/Font.h"
#include "FileSystem/DirectoryHelper.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(FontManager, ResourceManager);

	const char* defaultFontPath = GET_PACKAGE_PATH("\\Basic\\Font\\OpenSans-Light_SDF.zFont");

	void FontManager::Init(GameContext* _gameContext)
	{
		if (!s_instance)
		{
			Handle hFontManager("Font Manager", sizeof(FontManager));
			s_instance = new (hFontManager) FontManager();
			s_instance->m_gameContext = _gameContext;
		}

		s_instance->loadResource(defaultFontPath);

		s_instance->doLoadUnload();
	}

	void FontManager::Destroy()
	{
		s_instance->unloadResources();
	}

	ZE::Font* FontManager::GetDefaultFont()
	{
		return s_instance->getResource<Font>(defaultFontPath);
	}

	ZE::Handle FontManager::loadResource_Internal(const char* resourceFilePath)
	{
		return Font::LoadFont(resourceFilePath, m_gameContext);
	}

	void FontManager::preUnloadResource(Resource* _resource)
	{
		Font* pFont = _resource->m_hActual.getObject<Font>();
		pFont->release();
	}

	ZE::FontManager* FontManager::s_instance;

}