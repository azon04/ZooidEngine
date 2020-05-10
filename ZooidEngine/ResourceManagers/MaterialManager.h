#ifndef __ZE_MATERIAL_MANAGER__
#define __ZE_MATERIAL_MANAGER__

#include "ResourceManager.h"
#include "Utils/StringName.h"

namespace ZE {
	
	class MaterialManager : public ResourceManager
	{
		DEFINE_CLASS(MaterialManager);

	public:

		static MaterialManager* GetInstance();

		static void Init();
		static void Destroy();

	protected:
		// ResourceManager implementation
		virtual Handle loadResource_Internal(const char* resourceFilePath, ResourceCreateSettings* settings) override;
		virtual void preUnloadResource(Resource* _resource) override;

	private:
		static MaterialManager* s_instance;

		// Tokens
		static const StringName DIFFUSE;
		static const StringName SPECULAR;
		static const StringName NORMAL;
		static const StringName MASK;
		static const StringName ROUGHNESS_MAP;
		static const StringName METALIC_MAP;
		static const StringName AO_MAP;
		static const StringName SHININESS;
		static const StringName KA;
		static const StringName KD;
		static const StringName KS;
		static const StringName BLEND;
		static const StringName DOUBLE_SIDED;
		static const StringName METALIC;
		static const StringName ROUGHNESS;
		static const StringName REFLECTIVITY;
	};
}
#endif
