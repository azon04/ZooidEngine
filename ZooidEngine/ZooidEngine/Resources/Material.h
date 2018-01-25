#ifndef __ZE_MATERIAL_H__
#define __ZE_MATERIAL_H__

#include "ZEngineIncl.h"

#include "Utils/Array.h"
#include "Utils/PrimitiveTypes.h"
#include "Renderer/ShaderAction.h"

namespace ZE
{
	class IGPUTexture;
	class GameContext;

	enum class TextureType : UInt8
	{
		DIFFUSE = 1,
		SPECULAR = 2
	};

	struct MaterialTexture
	{
		IGPUTexture* texture;
		TextureType type;
	};

	class Material : public Object
	{
		DEFINE_CLASS(Material)

	public:

		Material() {}
		Array<MaterialTexture, true> m_textures;
		Float32 m_shininess;

		void Bind(ShaderAction& shaderAction);
	};
}
#endif
