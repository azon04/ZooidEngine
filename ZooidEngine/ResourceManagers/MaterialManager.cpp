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

	const ZE::StringName MaterialManager::DIFFUSE("diffuse");
	const ZE::StringName MaterialManager::SPECULAR("specular");
	const ZE::StringName MaterialManager::NORMAL("normal");
	const ZE::StringName MaterialManager::MASK("mask");
	const ZE::StringName MaterialManager::ROUGHNESS_MAP("roughnessMap");
	const ZE::StringName MaterialManager::METALIC_MAP("metalicMap");
	const ZE::StringName MaterialManager::AO_MAP("aoMap");
	const ZE::StringName MaterialManager::SHININESS("shininess");
	const ZE::StringName MaterialManager::KA("Ka");
	const ZE::StringName MaterialManager::KD("Kd");
	const ZE::StringName MaterialManager::KS("Ks");
	const ZE::StringName MaterialManager::BLEND("blend");
	const ZE::StringName MaterialManager::DOUBLE_SIDED("double_sided");
	const ZE::StringName MaterialManager::METALIC("metalic");
	const ZE::StringName MaterialManager::ROUGHNESS("roughness");
	const ZE::StringName MaterialManager::REFLECTIVITY("reflectivity");

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
			StringName bufferName(tokenBuffer);

			if (bufferName == DIFFUSE)
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
			else if (bufferName == SPECULAR)
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
			else if (bufferName == NORMAL)
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
			else if (bufferName == MASK)
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
			else if (bufferName == ROUGHNESS_MAP)
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
			else if (bufferName == METALIC_MAP)
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
			else if (bufferName == AO_MAP)
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
			else if (bufferName == SHININESS)
			{
				pMaterial->m_shininess = reader.readNextFloat();
			}
			else if (bufferName == KA)
			{
				pMaterial->m_Ka.setX(reader.readNextFloat());
				pMaterial->m_Ka.setY(reader.readNextFloat());
				pMaterial->m_Ka.setZ(reader.readNextFloat());
			}
			else if (bufferName == KD)
			{
				pMaterial->m_Kd.setX(reader.readNextFloat());
				pMaterial->m_Kd.setY(reader.readNextFloat());
				pMaterial->m_Kd.setZ(reader.readNextFloat());
			}
			else if (bufferName == KS)
			{
				pMaterial->m_Ks.setX(reader.readNextFloat());
				pMaterial->m_Ks.setY(reader.readNextFloat());
				pMaterial->m_Ks.setZ(reader.readNextFloat());
			}
			else if (bufferName == BLEND)
			{
				pMaterial->m_isBlend = true;
			}
			else if (bufferName == DOUBLE_SIDED)
			{
				pMaterial->m_bDoubleSided = true;
			}
			else if (bufferName == METALIC)
			{
				pMaterial->m_metalic = reader.readNextFloat();
			}
			else if (bufferName == ROUGHNESS)
			{
				pMaterial->m_roughness = reader.readNextFloat();
			}
			else if (bufferName == REFLECTIVITY)
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