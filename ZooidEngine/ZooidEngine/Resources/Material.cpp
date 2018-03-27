#include "Material.h"
#include "Utils/String.h"

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
				iDiffuse++;
				break;
			case TextureType::SPECULAR:
				varName += "specularMap";
				iSpecular++;
				break;
			default:
				break;
			}
			shaderAction.SetShaderTextureVar(varName.c_str(), matTexture.texture, i);
		}

		shaderAction.SetShaderFloatVar("material.diffuseMapBound", iDiffuse > 0 ? 1.0f : 0.0f);
		shaderAction.SetShaderFloatVar("material.specularMapBound", iSpecular > 0 ? 1.0f : 0.0f);

		shaderAction.SetShaderFloatVar("material.shininess", m_shininess);
		shaderAction.SetShaderVec3Var("material.Ka", m_Ka);
		shaderAction.SetShaderVec3Var("material.Kd", m_Kd);
		shaderAction.SetShaderVec3Var("material.Ks", m_Ks);
	}

}