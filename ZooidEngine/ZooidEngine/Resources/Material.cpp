#include "Material.h"

#include <stdio.h>

namespace ZE
{
	IMPLEMENT_CLASS_0(Material);

	void Material::Bind(ShaderAction& shaderAction)
	{
		int iDiffuse = 0;
		int iSpecular = 0;
		for (int i = 0; i < m_textures.length(); ++i)
		{
			MaterialTexture& matTexture = m_textures[i];
			String varName(1024);
			varName += "material.";
			switch (matTexture.type)
			{
			case TextureType::DIFFUSE:
				varName += "diffuseMap";
				break;
			case TextureType::SPECULAR:
				varName += "specularMap";
				break;
			default:
				break;
			}
			shaderAction.SetShaderTextureVar(varName.c_str(), matTexture.texture, i);
		}
		shaderAction.SetShaderFloatVar("material.shininess", m_shininess);
	}

}