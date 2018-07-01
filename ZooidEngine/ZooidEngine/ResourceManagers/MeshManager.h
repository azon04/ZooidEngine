#ifndef __ZE_MESH_MANAGER_H__
#define __ZE_MESH_MANAGER_H__

#include "ResourceManager.h"

namespace ZE
{
	class FileReader;
	class Mesh;

	class MeshManager : public ResourceManager
	{
		DEFINE_CLASS(MeshManager)

	public:

		static void Init();
		static void Destroy();
		static MeshManager* GetInstance();

	protected:
		// ResourceManager implementation
		virtual Handle loadResource_Internal(const char* resourceFilePath) override;
		virtual void preUnloadResource(Resource* _resource) override;

	private:
		static MeshManager* s_instance;

		void loadPhysicsBodySetup(FileReader* fileReader, Mesh* pMesh);

	};
}
#endif
