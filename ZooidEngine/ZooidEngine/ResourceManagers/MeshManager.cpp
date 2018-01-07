#include "MeshManager.h"

#include "Resources/Mesh.h"
#include "FileSystem/FileReader.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(MeshManager, ResourceManager)

	ZE::Handle MeshManager::loadResource_Internal(const char* resourceFilePath)
	{
		Handle meshHandle("Mesh Handle", sizeof(Mesh));
		Mesh* pMesh = new(meshHandle) Mesh();

		FileReader reader(resourceFilePath);

		if (reader.isValid())
		{
			// #TODO Parse the file
		}

		return Handle();
	}

	void MeshManager::preUnloadResource(Resource* _resource)
	{

	}

}