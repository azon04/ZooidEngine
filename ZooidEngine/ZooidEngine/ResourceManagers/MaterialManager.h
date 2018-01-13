#ifndef __ZE_MATERIAL_MANAGER__
#define __ZE_MATERIAL_MANAGER__

#include "ResourceManager.h"

namespace ZE {
	
	class MaterialManager : public ResourceManager
	{
		DEFINE_CLASS(MaterialManager);

	private:

		static MaterialManager* s_instance;

	public:

		static MaterialManager* getInstance();

		static void Init();
		static void Destroy();

		virtual Handle loadResource_Internal(const char* resourceFilePath) override;
		virtual void preUnloadResource(Resource* _resource) override;
	};
}
#endif
