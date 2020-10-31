#include "Material.h"
#include "Utils/String.h"

#include "Renderer/IShader.h"
#include "Renderer/IGPUTexture.h"

#include <stdio.h>

namespace ZE
{
	IMPLEMENT_CLASS_0(Material);

	void Material::BindForShadow(IShaderChain* shaderChain)
	{
		int iMask = 0;
		for (int i = 0; i < m_textures.length(); ++i)
		{
			MaterialTexture& matTexture = m_textures[i];
			switch (matTexture.type)
			{
			case TextureType::MASK:
				shaderChain->setTexture("material.maskMap", matTexture.texture, i);
				matTexture.texture->bind();
				iMask++;
				break;
			default:
				break;
			}
		}

		shaderChain->setInt("material.maskMapBound", iMask > 0);
	}

	void Material::Bind(IShaderChain* shaderChain)
	{
		int iDiffuse = 0;
		int iSpecular = 0;
		int iNormal = 0;
		int iMask = 0;
		int iMetalic = 0;
		int iRoughness = 0;
		int iAO = 0;
		for (int i = 0; i < m_textures.length(); ++i)
		{
			MaterialTexture& matTexture = m_textures[i];
			switch (matTexture.type)
			{
			case TextureType::DIFFUSE:
				shaderChain->setTexture("material.diffuseMap", matTexture.texture, i);
				iDiffuse++;
				break;
			case TextureType::SPECULAR:
				shaderChain->setTexture("material.specularMap", matTexture.texture, i);
				iSpecular++;
				break;
			case TextureType::NORMAL:
				shaderChain->setTexture("material.normalMap", matTexture.texture, i);
				iNormal++;
				break;
			case TextureType::MASK:
				shaderChain->setTexture("material.maskMap", matTexture.texture, i);
				iMask++;
				break;
			case TextureType::METALIC:
				shaderChain->setTexture("material.metalicMap", matTexture.texture, i);
				iMetalic++;
				break;
			case TextureType::ROUGHNESS:
				shaderChain->setTexture("material.roughnessMap", matTexture.texture, i);
				iRoughness++;
				break;;
			case TextureType::OCCLUSSION:
				shaderChain->setTexture("material.aoMap", matTexture.texture, i);
				iAO++;
				break;
			default:
				break;
			}
			matTexture.texture->bind();
		}

		shaderChain->setFloat("material.diffuseMapBound", iDiffuse > 0 ? 1.0f : 0.0f);
		shaderChain->setFloat("material.specularMapBound", iSpecular > 0 ? 1.0f : 0.0f);
		shaderChain->setFloat("material.normalMapBound", iNormal > 0 ? 1.0f : 0.0f);
		shaderChain->setInt("material.maskMapBound", iMask > 0);
		shaderChain->setFloat("material.metalicMapBound", iMetalic > 0 ? 1.0f : 0.0f);
		shaderChain->setFloat("material.roughnessMapBound", iRoughness > 0 ? 1.0f : 0.0f);
		shaderChain->setFloat("material.aoMapBound", iAO > 0 ? 1.0f : 0.0f);

		shaderChain->setFloat("material.shininess", m_shininess);
		
		// Set PBR material variables
		shaderChain->setFloat("material.metalic", m_metalic);
		shaderChain->setFloat("material.roughness", m_roughness);
		shaderChain->setFloat("material.reflectivity", m_reflectivity);

		shaderChain->setVec3("material.Ka", m_Ka);
		shaderChain->setVec3("material.Kd", m_Kd);
		shaderChain->setVec3("material.Ks", m_Ks);
	}

	ZE::IGPUTexture* Material::getTextureType(TextureType textureType) const
	{
		for (Int32 i = 0; i < m_textures.size(); i++)
		{
			if (m_textures[i].type == textureType) { return m_textures[i].texture; }
		}
		return nullptr;
	}

}