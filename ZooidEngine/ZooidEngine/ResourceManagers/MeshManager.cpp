#include "MeshManager.h"

#include "BufferManager.h"
#include "MaterialManager.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(MeshManager, ResourceManager)

	MeshManager* MeshManager::s_instance = nullptr;

	void MeshManager::Init()
	{
		Handle hMeshManager("Mesh Manager", sizeof(MeshManager));
		s_instance = new(hMeshManager) MeshManager();

		s_instance->loadResource(GetPackageAssetPath("Basic", "Mesh", "Crate.meshz").c_str());
	}

	void MeshManager::Destroy()
	{
		if(s_instance)
			s_instance->unloadResources();
		delete s_instance;
		s_instance = nullptr;
	}

	ZE::MeshManager* MeshManager::getInstance()
	{
		return s_instance;
	}

	ZE::Handle MeshManager::loadResource_Internal(const char* resourceFilePath)
	{
		Handle meshHandle("Mesh Handle", sizeof(Mesh));
		Mesh* pMesh;

		FileReader reader(resourceFilePath);

		if (!reader.isValid())
		{
			ZEWARNING("Mesh file not found : %s", resourceFilePath);
			return meshHandle;
		}
		
		pMesh = new(meshHandle) Mesh();
		while (!reader.eof())
		{
			char tokenBuffer[126];
			reader.readNextString(tokenBuffer);
			if (StringFunc::Compare(tokenBuffer, "vbuff") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hVertexBuffer = BufferManager::getInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hVertexBuffer.isValid())
				{
					pMesh->m_bufferArray = hVertexBuffer.getObject<GPUBufferArray>();
				}
			}
			else if (StringFunc::Compare(tokenBuffer, "mat") == 0)
			{
				reader.readNextString(tokenBuffer);

				Handle hMaterial = MaterialManager::getInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hMaterial.isValid())
				{
					pMesh->m_material = hMaterial.getObject<Material>();
				}

			}
		}

		reader.close();
		
		return meshHandle;
	}

	void MeshManager::preUnloadResource(Resource* _resource)
	{

	}

}