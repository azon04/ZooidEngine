#ifndef __ZE_MATERIAL_H__
#define __ZE_MATERIAL_H__

#include "GameObjectModel/Object.h"
#include "Utils/Array.h"
#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"

namespace ZE
{
	class IGPUTexture;
	class GameContext;
	class IShaderChain;

	enum class TextureType : UInt8
	{
		DIFFUSE = 1,
		SPECULAR = 2,
		NORMAL = 3,
		MASK = 4,
		METALIC = 5,
		ROUGHNESS = 6,
		OCCLUSSION = 7
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
			: m_Ka(0.05f, 0.05f, 0.05f),
			m_Kd(1.0f, 1.0f, 1.0f),
			m_Ks(1.0f, 1.0f, 1.0f),
			m_shininess(0.0f),
			m_metalic(0.0f),
			m_roughness(1.0f),
			m_reflectivity(0.04f),
			m_isBlend(false),
			m_bDoubleSided(false)
		{
		}

		void Bind(IShaderChain* shaderChain);

		// Does material have transparency?
		bool IsBlend() const { return m_isBlend; }
		bool HasMask() const { return getTextureType(TextureType::MASK) != nullptr; }
		bool IsDoubleSided() const { return m_bDoubleSided; }

		UInt32 getTextureCount() const { return m_textures.size(); }
		IGPUTexture* getTextureType(TextureType textureType) const;

	protected:
		Vector3 m_Ka;
		Vector3 m_Kd;
		Vector3 m_Ks;
		Array<MaterialTexture, true> m_textures;
		Float32 m_shininess;

		// PBR variables
		Float32 m_metalic;
		Float32 m_roughness;
		Float32 m_reflectivity;

		bool m_isBlend;
		bool m_bDoubleSided;

	};
}
#endif
