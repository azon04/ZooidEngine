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

	MaterialManager* MaterialManager::GetInstance()
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

	ZE::Handle MaterialManager::loadResource_Internal(const char* resourceFilePath, ResourceCreateSettings* settings)
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
			tokenBuffer[0] = '\0';
			reader.readNextString(tokenBuffer);

			if (StringFunc::Compare(tokenBuffer, "diffuse") == 0)
			{
				reader.readNextString(tokenBuffer);
				TextureResourceCreateSettings settings;
				settings.bGammaCorrectedImage = true;
				Handle hTexture = TextureManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str(), &settings);
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
				Handle hTexture = TextureManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::SPECULAR;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "normal") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hTexture = TextureManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::NORMAL;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "mask") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hTexture = TextureManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::MASK;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "roughnessMap") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hTexture = TextureManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::ROUGHNESS;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "metalicMap") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hTexture = TextureManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::METALIC;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "aoMap") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hTexture = TextureManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hTexture.isValid())
				{
					MaterialTexture texture;
					texture.texture = hTexture.getObject<IGPUTexture>();
					texture.type = TextureType::OCCLUSSION;
					pMaterial->m_textures.push_back(texture);
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "shininess") == 0)
			{
				pMaterial->m_shininess = reader.readNextFloat();
			}
			else if (StringFunc::Compare(tokenBuffer, "Ka") == 0)
			{
				pMaterial->m_Ka.setX(reader.readNextFloat());
				pMaterial->m_Ka.setY(reader.readNextFloat());
				pMaterial->m_Ka.setZ(reader.readNextFloat());
			}
			else if (StringFunc::Compare(tokenBuffer, "Kd") == 0)
			{
				pMaterial->m_Kd.setX(reader.readNextFloat());
				pMaterial->m_Kd.setY(reader.readNextFloat());
				pMaterial->m_Kd.setZ(reader.readNextFloat());
			}
			else if (StringFunc::Compare(tokenBuffer, "Ks") == 0)
			{
				pMaterial->m_Ks.setX(reader.readNextFloat());
				pMaterial->m_Ks.setY(reader.readNextFloat());
				pMaterial->m_Ks.setZ(reader.readNextFloat());
			}
			else if (StringFunc::Compare(tokenBuffer, "blend") == 0)
			{
				pMaterial->m_isBlend = true;
			}
			else if (StringFunc::Compare(tokenBuffer, "double_sided") == 0)
			{
				pMaterial->m_bDoubleSided = true;
			}
			else if (StringFunc::Compare(tokenBuffer, "metalic") == 0)
			{
				pMaterial->m_metalic = reader.readNextFloat();
			}
			else if (StringFunc::Compare(tokenBuffer, "roughness") == 0)
			{
				pMaterial->m_roughness = reader.readNextFloat();
			}
			else if (StringFunc::Compare(tokenBuffer, "reflectivity") == 0)
			{
				pMaterial->m_reflectivity = reader.readNextFloat();
			}
		}

		reader.close();

		return hMaterial;
		
	}

	void MaterialManager::preUnloadResource(Resource* _resource)
	{

	}

}