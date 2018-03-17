#include "TextureManager.h"

#include "Memory/Handle.h"

#include "Resources/Texture.h"
#include "Renderer/IGPUTexture.h"
#include "FileSystem/DirectoryHelper.h"

#include "ZEGameContext.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IRenderer.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(TextureManager, ResourceManager)

	TextureManager* TextureManager::s_instance = nullptr;

	void TextureManager::Init(GameContext* _gameContext)
	{
		if (!s_instance)
		{
			Handle textureManagerHandle("TEXTURE_MANAGER", sizeof(TextureManager));
			s_instance = new (textureManagerHandle) TextureManager();
			s_instance->m_gameContext = _gameContext;
		}

		s_instance->loadResourceAsync(GetPackageAssetPath("Basic", "Texture", "container2.png").c_str());

		s_instance->doLoadUnload();
	}

	TextureManager* TextureManager::getInstance()
	{
		return s_instance;
	}

	void TextureManager::Destroy()
	{
		s_instance->unloadResource(GetPackageAssetPath("Basic", "Texture", "container2.png").c_str());

		s_instance->unloadResources();
	}

	ZE::Handle TextureManager::loadResource_Internal(const char* resourceFilePath)
	{
		Handle hGPUTexture(sizeof(IGPUTexture));
		Handle hCPUTexture = Texture::loadTexture(resourceFilePath);
		if (hCPUTexture.isValid())
		{
			m_gameContext->getRenderer()->AcquireRenderThreadOwnership();

			hGPUTexture = m_gameContext->getRenderZooid()->CreateRenderTexture();
			IGPUTexture* pGPUTexture = hGPUTexture.getObject<IGPUTexture>();
			pGPUTexture->FromTexture(hCPUTexture.getObject<Texture>());

			m_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
		}

		// Clean the CPU texture
		// #TODO Do we need to save CPU texture?
		hCPUTexture.getObject<Texture>()->release();
		hCPUTexture.release();

		return hGPUTexture;
	}

	void TextureManager::preUnloadResource(Resource* _resource)
	{
		IGPUTexture* pGPUTexture = _resource->m_hActual.getObject<IGPUTexture>();
		pGPUTexture->release();
	}

}