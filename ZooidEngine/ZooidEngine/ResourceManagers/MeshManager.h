#ifndef __ZE_MESH_MANAGER_H__
#define __ZE_MESH_MANAGER_H__

#include "ResourceManager.h"

namespace ZE
{
	class MeshManager : public ResourceManager
	{
		DEFINE_CLASS(MeshManager)

	public:

		virtual Handle loadResource_Internal(const char* resourceFilePath) override;
		virtual void preUnloadResource(Resource* _resource) override;
	};
}
#endif
