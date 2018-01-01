#include "TextureManager.h"

#include "MemoryManagement/Handle.h"

#include "Resources/Texture.h"
#include "Renderer/GPUTexture.h"

namespace ZE
{
	TextureManager* TextureManager::s_instance = nullptr;

	void TextureManager::Init()
	{
		if (!s_instance)
		{
			Handle textureManagerHandle("TEXTURE_MANAGER", sizeof(TextureManager));
			s_instance = new (textureManagerHandle) TextureManager();
		}

		s_instance->registerResourceToLoad("../Resources/Textures/container2.png");

		s_instance->doLoadUnload();
	}

	TextureManager* TextureManager::getInstance()
	{
		return s_instance;
	}

	void TextureManager::Destroy()
	{
		s_instance->registerResourceToUnLoad("../Resources/Textures/container2.png");

		s_instance->unloadResources();
	}

	ZE::Handle TextureManager::loadResource(const char* resourceFilePath)
	{
		Handle hGPUTexture(sizeof(GPUTexture));
		Handle hCPUTexture = Texture::loadTexture(resourceFilePath);
		if (hCPUTexture.isValid())
		{
			GPUTexture* pGPUTexture = new (hGPUTexture) GPUTexture();
			pGPUTexture->FromTexture(hCPUTexture.getObject<Texture>());
		}

		// Clean the CPU texture
		// #TODO Do we need to save CPU texture?
		hCPUTexture.getObject<Texture>()->release();
		hCPUTexture.release();

		return hGPUTexture;
	}

	void TextureManager::preUnloadResource(Resource* _resource)
	{
		GPUTexture* pGPUTexture = _resource->m_hActual.getObject<GPUTexture>();
		pGPUTexture->release();
	}

}