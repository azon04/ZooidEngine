#include "MaterialManager.h"

#include "TextureManager.h"

#include "Resources/Material.h"
#include "Renderer/IGPUTexture.h"

#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"
#include "Utils/StringFunc.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(MaterialManager, ResourceManager);

	MaterialManager* MaterialManager::s_instance = nullptr;

	MaterialManager* MaterialManager::getInstance()
	{
		return s_instance;
	}

	void MaterialManager::Init()
	{
		Handle hMaterialManager("Material Manager", sizeof(MaterialManager));
		s_instance = new(hMaterialManager) MaterialManager();
	}

	void MaterialManager::Destroy()
	{
		s_instance->unloadResources();
		delete s_instance;
		s_instance = nullptr;
	}

	ZE::Handle MaterialManager::loadResource_Internal(const char* resourceFilePath)
	{
		Handle hMaterial("Material", sizeof(Material));

		FileReader reader(resourceFilePath);
		if (!reader.isValid())
		{
			ZEWARNING("Texture file not found : %s", resourceFilePath);
			return hMaterial;
		}

		char tokenBuffer[256];

		Material* pMaterial = new(hMaterial) Material();

		while (!reader.eof())
		{
			reader.readNextString(tokenBuffer);

			if (StringFunc::Compare(tokenBuffer, "diffuse") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hTexture = TextureManager::getInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::DIFFUSE;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "specular") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hTexture = TextureManager::getInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::SPECULAR;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "shininess") == 0)
			{
				pMaterial->m_shininess = reader.readNextFloat();
			}
		}

		reader.close();

		return hMaterial;
		
	}

	void MaterialManager::preUnloadResource(Resource* _resource)
	{

	}

}