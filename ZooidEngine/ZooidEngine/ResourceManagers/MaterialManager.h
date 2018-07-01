#ifndef __ZE_MATERIAL_MANAGER__
#define __ZE_MATERIAL_MANAGER__

#include "ResourceManager.h"

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
		virtual Handle loadResource_Internal(const char* resourceFilePath) override;
		virtual void preUnloadResource(Resource* _resource) override;

	private:
		static MaterialManager* s_instance;

	};
}
#endif
