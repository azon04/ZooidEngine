#include "MeshManager.h"

#include "BufferManager.h"
#include "MaterialManager.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"

#include "Physics/Physics.h"

#define RETURN_SHAPE_IF(shapeType, s) \
	if (StringFunc::Compare(s, #shapeType ) == 0) \
	{ \
		return shapeType; \
	}

namespace ZE
{
	IMPLEMENT_CLASS_1(MeshManager, ResourceManager)

	MeshManager* MeshManager::s_instance = nullptr;

	void MeshManager::Init()
	{
		Handle hMeshManager("Mesh Manager", sizeof(MeshManager));
		s_instance = new(hMeshManager) MeshManager();
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

	PhysicsShape GetShapeFromString(const char* shapeName)
	{
		RETURN_SHAPE_IF(BOX, shapeName)
		RETURN_SHAPE_IF(SPHERE, shapeName)
		RETURN_SHAPE_IF(CAPSULE, shapeName)
		RETURN_SHAPE_IF(PLANE, shapeName)
		RETURN_SHAPE_IF(CONVEX_MESHES, shapeName)
		RETURN_SHAPE_IF(TRIANGLE_MESHES, shapeName)
		RETURN_SHAPE_IF(HEIGHT_FIELDS, shapeName)
	}

	void loadPhysicsBodySetup(FileReader& fileReader, Mesh* pMesh)
	{
		Handle hPhysicsBodySetup("Physics Body Setup", sizeof(PhysicsBodySetup));
		PhysicsBodySetup* pPhysicsBodySetup = new(hPhysicsBodySetup) PhysicsBodySetup();

		char tokenBuffer[64];
		
		// Read physic data type : single/file/multi
		fileReader.readNextString(tokenBuffer);

		if (StringFunc::Compare(tokenBuffer, "single") == 0)
		{
			PhysicsBodyDesc bodyDesc;
			fileReader.readNextString(tokenBuffer);
			bodyDesc.ShapeType = GetShapeFromString(tokenBuffer);
			switch (bodyDesc.ShapeType)
			{
			case BOX:
				bodyDesc.HalfExtent.setX(fileReader.readNextFloat());
				bodyDesc.HalfExtent.setY(fileReader.readNextFloat());
				bodyDesc.HalfExtent.setZ(fileReader.readNextFloat());
				break;
			case SPHERE:
				bodyDesc.Radius = fileReader.readNextFloat();
				break;
			case CAPSULE:
				bodyDesc.Radius = fileReader.readNextFloat();
				bodyDesc.HalfHeight = fileReader.readNextFloat();
				break;
			case PLANE:
				break;
			case CONVEX_MESHES:
				// #TODO Read buffer
				break;
			case TRIANGLE_MESHES:
				// #TODO Read buffer
				break;
			case HEIGHT_FIELDS:
				// #TODO Read HeightField
				break;
			}
			pPhysicsBodySetup->m_bodies.push_back(bodyDesc);
		}
		else if (StringFunc::Compare(tokenBuffer, "multi") == 0)
		{
			// #TODO need to implement multi descriptor
		}
		else if (StringFunc::Compare(tokenBuffer, "file") == 0)
		{
			// #TODO need to implement file descriptor for this
		}

		pMesh->m_hPhysicsBodySetup = hPhysicsBodySetup;
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
					pMesh->m_bufferArray = hVertexBuffer.getObject<IGPUBufferArray>();
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
			else if (StringFunc::Compare(tokenBuffer, "double_side") == 0)
			{
				pMesh->m_doubleSided = true;
			}
			else if (StringFunc::Compare(tokenBuffer, "physics") == 0)
			{
				loadPhysicsBodySetup(reader, pMesh);
			}
		}

		reader.close();
		
		return meshHandle;
	}

	void MeshManager::preUnloadResource(Resource* _resource)
	{
		Mesh* pMesh = _resource->m_hActual.getObject<Mesh>();

		if (pMesh && pMesh->m_hPhysicsBodySetup.isValid())
		{
			pMesh->m_hPhysicsBodySetup.release();
		}
	}

}