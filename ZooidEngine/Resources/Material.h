#ifndef __ZE_MATERIAL_H__
#define __ZE_MATERIAL_H__

#include "Utils/Array.h"
#include "Utils/PrimitiveTypes.h"
#include "Renderer/ShaderAction.h"
#include "Math/Vector3.h"

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

		friend class MaterialManager;
	public:

		Material() 
			: m_Ka(1.0f, 1.0f, 1.0f),
			m_Kd(1.0f, 1.0f, 1.0f),
			m_Ks(1.0f, 1.0f, 1.0f),
			m_isBlend(false)
		{
		}

		void Bind(ShaderAction& shaderAction);

		// Does material have transparency?
		bool IsBlend() { return m_isBlend; }

	protected:
		Vector3 m_Ka;
		Vector3 m_Kd;
		Vector3 m_Ks;
		Array<MaterialTexture, true> m_textures;
		Float32 m_shininess;
		bool m_isBlend;

	};
}
#endif
