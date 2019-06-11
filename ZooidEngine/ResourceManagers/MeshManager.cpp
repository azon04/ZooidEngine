#include "MeshManager.h"

#include "BufferManager.h"
#include "MaterialManager.h"
#include "SkeletonManager.h"

#include "Resources/Mesh.h"
#include "Resources/Material.h"

#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"

#include "Renderer/IRenderer.h"

#include "Physics/Physics.h"

#include "ZEGameContext.h"

#define RETURN_SHAPE_IF(shapeType, s) \
	if (StringFunc::Compare(s, #shapeType ) == 0) \
	{ \
		return shapeType; \
	}

namespace ZE
{
	IMPLEMENT_CLASS_1(MeshManager, ResourceManager)

	struct MeshSkinData
	{
		Float32 Position[3];
		Float32 Normal[3];
		Float32 TexCoord[2];
		Int32 BoneIDs[4];
		Float32 BoneWeights[4];
	};

	struct MeshSkinDataWithTangent
	{
		Float32 Position[3];
		Float32 Normal[3];
		Float32 TexCoord[2];
		Int32 BoneIDs[4];
		Float32 BoneWeights[4];
		Float32 Tangent[3];
	};

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

	ZE::MeshManager* MeshManager::GetInstance()
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
		return PHYSICS_SHAPE_NONE;
	}

	void MeshManager::loadPhysicsBodySetup(FileReader* fileReader, Mesh* pMesh)
	{
		Handle hPhysicsBodySetup("Physics Body Setup", sizeof(PhysicsBodySetup));
		PhysicsBodySetup* pPhysicsBodySetup = new(hPhysicsBodySetup) PhysicsBodySetup();

		char tokenBuffer[64];
		
		// Read physic data type : single/file/multi
		fileReader->readNextString(tokenBuffer);

		if (StringFunc::Compare(tokenBuffer, "single") == 0)
		{
			PhysicsBodyDesc bodyDesc;
			fileReader->readNextString(tokenBuffer);
			bodyDesc.ShapeType = GetShapeFromString(tokenBuffer);
			switch (bodyDesc.ShapeType)
			{
			case BOX:
				bodyDesc.HalfExtent.setX(fileReader->readNextFloat());
				bodyDesc.HalfExtent.setY(fileReader->readNextFloat());
				bodyDesc.HalfExtent.setZ(fileReader->readNextFloat());
				break;
			case SPHERE:
				bodyDesc.Radius = fileReader->readNextFloat();
				break;
			case CAPSULE:
				bodyDesc.Radius = fileReader->readNextFloat();
				bodyDesc.HalfHeight = fileReader->readNextFloat();
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

			// Mass expected
			fileReader->readNextString(tokenBuffer);

			if (StringFunc::Compare(tokenBuffer, "Mass") == 0)
			{
				pPhysicsBodySetup->Mass = fileReader->readNextFloat();
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

	void MeshManager::loadBuffers(FileReader* fileReader, Mesh* pMesh)
	{
		char buffer[256];

		// READ VERTEX BUFFER
		fileReader->readNextString(buffer);
		int bufferLayoutType = getBufferLayoutByString(buffer);
		if (bufferLayoutType == -1)
		{
			ZEINFO("BufferManager: Can't find buffer layout type for %s", buffer);
			return;
		}

		Handle hVertexBufferData("Buffer Data", sizeof(BufferData));
		BufferData* pVertexBuffer = new(hVertexBufferData) BufferData(VERTEX_BUFFER);
		pVertexBuffer->setBufferLayout(bufferLayoutType);

		int numVertex = fileReader->readNextInt();

		Handle dataHandle;

		if (bufferLayoutType == BUFFER_LAYOUT_V3_N3_TC2_SKIN)
		{
			dataHandle = Handle("Data", sizeof(MeshSkinData) * numVertex);
			MeshSkinData* pData = new(dataHandle) MeshSkinData[numVertex];

			Vector3 minVertex(9999.9f);
			Vector3 maxVertex(-9999.9f);
			Vector3 centerVertex(0.0f);

			for (int i = 0; i < numVertex; i++)
			{
				pData[i].Position[0] = fileReader->readNextFloat();
				pData[i].Position[1] = fileReader->readNextFloat();
				pData[i].Position[2] = fileReader->readNextFloat();
				pData[i].Normal[0] = fileReader->readNextFloat();
				pData[i].Normal[1] = fileReader->readNextFloat();
				pData[i].Normal[2] = fileReader->readNextFloat();
				pData[i].TexCoord[0] = fileReader->readNextFloat();
				pData[i].TexCoord[1] = fileReader->readNextFloat();
				pData[i].BoneIDs[0] = fileReader->readNextInt();
				pData[i].BoneIDs[1] = fileReader->readNextInt();
				pData[i].BoneIDs[2] = fileReader->readNextInt();
				pData[i].BoneIDs[3] = fileReader->readNextInt();
				pData[i].BoneWeights[0] = fileReader->readNextFloat();
				pData[i].BoneWeights[1] = fileReader->readNextFloat();
				pData[i].BoneWeights[2] = fileReader->readNextFloat();
				pData[i].BoneWeights[3] = fileReader->readNextFloat();

				if (minVertex.m_x > pData[i].Position[0])
				{
					minVertex.m_x = pData[i].Position[0];
				}

				if (minVertex.m_y > pData[i].Position[1])
				{
					minVertex.m_y = pData[i].Position[1];
				}

				if (minVertex.m_z > pData[i].Position[2])
				{
					minVertex.m_z = pData[i].Position[2];
				}

				if (maxVertex.m_x < pData[i].Position[0])
				{
					maxVertex.m_x = pData[i].Position[0];
				}

				if (maxVertex.m_y < pData[i].Position[1])
				{
					maxVertex.m_y = pData[i].Position[1];
				}

				if (maxVertex.m_z < pData[i].Position[2])
				{
					maxVertex.m_z = pData[i].Position[2];
				}

				centerVertex = centerVertex + Vector3(pData[i].Position);
			}

			centerVertex = centerVertex / (Float32)numVertex;
			pMesh->m_boxMin = minVertex;
			pMesh->m_boxMax = maxVertex;
			pMesh->m_centerOffset = centerVertex;
			Vector3 centerToMax = maxVertex - centerVertex;
			pMesh->m_radius = centerToMax.length();

			pVertexBuffer->SetData(pData, sizeof(MeshSkinData), numVertex);
		}
		else if (bufferLayoutType == BUFFER_LAYOUT_V3_N3_T3_TC2_SKIN)
		{
			dataHandle = Handle("Data", sizeof(MeshSkinDataWithTangent) * numVertex);
			MeshSkinDataWithTangent* pData = new(dataHandle) MeshSkinDataWithTangent[numVertex];

			Vector3 minVertex(9999.9f);
			Vector3 maxVertex(-9999.9f);
			Vector3 centerVertex(0.0f);

			for (int i = 0; i < numVertex; i++)
			{
				pData[i].Position[0] = fileReader->readNextFloat();
				pData[i].Position[1] = fileReader->readNextFloat();
				pData[i].Position[2] = fileReader->readNextFloat();
				pData[i].Normal[0] = fileReader->readNextFloat();
				pData[i].Normal[1] = fileReader->readNextFloat();
				pData[i].Normal[2] = fileReader->readNextFloat();

				pData[i].TexCoord[0] = fileReader->readNextFloat();
				pData[i].TexCoord[1] = fileReader->readNextFloat();

				pData[i].Tangent[0] = fileReader->readNextFloat();
				pData[i].Tangent[1] = fileReader->readNextFloat();
				pData[i].Tangent[2] = fileReader->readNextFloat();

				pData[i].BoneIDs[0] = fileReader->readNextInt();
				pData[i].BoneIDs[1] = fileReader->readNextInt();
				pData[i].BoneIDs[2] = fileReader->readNextInt();
				pData[i].BoneIDs[3] = fileReader->readNextInt();
				pData[i].BoneWeights[0] = fileReader->readNextFloat();
				pData[i].BoneWeights[1] = fileReader->readNextFloat();
				pData[i].BoneWeights[2] = fileReader->readNextFloat();
				pData[i].BoneWeights[3] = fileReader->readNextFloat();

				if (minVertex.m_x > pData[i].Position[0])
				{
					minVertex.m_x = pData[i].Position[0];
				}

				if (minVertex.m_y > pData[i].Position[1])
				{
					minVertex.m_y = pData[i].Position[1];
				}

				if (minVertex.m_z > pData[i].Position[2])
				{
					minVertex.m_z = pData[i].Position[2];
				}

				if (maxVertex.m_x < pData[i].Position[0])
				{
					maxVertex.m_x = pData[i].Position[0];
				}

				if (maxVertex.m_y < pData[i].Position[1])
				{
					maxVertex.m_y = pData[i].Position[1];
				}

				if (maxVertex.m_z < pData[i].Position[2])
				{
					maxVertex.m_z = pData[i].Position[2];
				}

				centerVertex = centerVertex + Vector3(pData[i].Position);
			}

			centerVertex = centerVertex / (Float32)numVertex;
			pMesh->m_boxMin = minVertex;
			pMesh->m_boxMax = maxVertex;
			pMesh->m_centerOffset = centerVertex;
			Vector3 centerToMax = maxVertex - centerVertex;
			pMesh->m_radius = centerToMax.length();

			pVertexBuffer->SetData(pData, sizeof(MeshSkinDataWithTangent), numVertex);
		}
		else
		{
			int dataPerVertex = 6;
			switch (bufferLayoutType)
			{
			case BUFFER_LAYOUT_V3_C3:
				dataPerVertex = 6;
				break;
			case BUFFER_LAYOUT_V3_N3_TC2:
				dataPerVertex = 8;
				break;
			case BUFFER_LAYOUT_V3_N3_T3_TC2:
				dataPerVertex = 11;
				break;
			case BUFFER_LAYOUT_V3_N3_TC2_SKIN:
				dataPerVertex = 16;
				break;
			default:
				break;
			}

			int totalSize = dataPerVertex * numVertex;

			dataHandle = Handle("Data", sizeof(Float32) * totalSize);
			Float32* pData = new(dataHandle) Float32[totalSize];

			Vector3 minVertex(9999.9f);
			Vector3 maxVertex(-9999.9f);
			Vector3 centerVertex(0.0f);

			for (int i = 0; i < totalSize; ++i)
			{
				pData[i] = fileReader->readNextFloat();

				if (i % dataPerVertex == 0)
				{
					centerVertex.m_x += pData[i];
					if (minVertex.m_x > pData[i])
					{
						minVertex.m_x = pData[i];
					}

					if (maxVertex.m_x < pData[i])
					{
						maxVertex.m_x = pData[i];
					}
				}
				else if (i % dataPerVertex == 1)
				{
					centerVertex.m_y += pData[i];
					if (minVertex.m_y > pData[i])
					{
						minVertex.m_y = pData[i];
					}

					if (maxVertex.m_y < pData[i])
					{
						maxVertex.m_y = pData[i];
					}
				}
				else if (i % dataPerVertex == 2)
				{
					centerVertex.m_z += pData[i];
					if (minVertex.m_z > pData[i])
					{
						minVertex.m_z = pData[i];
					}

					if (maxVertex.m_z < pData[i])
					{
						maxVertex.m_z = pData[i];
					}
				}
			}

			centerVertex = centerVertex / (Float32)numVertex;
			pMesh->m_boxMin = minVertex;
			pMesh->m_boxMax = maxVertex;
			pMesh->m_centerOffset = centerVertex;
			Vector3 centerToMax = maxVertex - centerVertex;
			pMesh->m_radius = centerToMax.length();

			pVertexBuffer->SetData(pData, sizeof(Float32) * dataPerVertex, numVertex);
		}


		// READ OPTIONAL INDEX BUFFER
		Handle hIndexBufferData("Buffer Data", sizeof(BufferData));
		BufferData* pIndexBuffer = nullptr;
		Handle indexDataHandle;

		fileReader->readNextString(buffer);
		if (StringFunc::Compare(buffer, "INDICE_BUFFER_NONE") != 0)
		{
			int numIndex = fileReader->readNextInt();
			indexDataHandle = Handle("Data", sizeof(Int32) * numIndex);
			UInt32* indexData = new(indexDataHandle) UInt32[numIndex];

			for (int i = 0; i < numIndex; ++i)
			{
				indexData[i] = fileReader->readNextInt();
			}

			pIndexBuffer = new(hIndexBufferData) BufferData(INDEX_BUFFER);
			pIndexBuffer->SetData(indexData, sizeof(UInt32), numIndex);
		}

		fileReader->close();

		ScopedRenderThreadOwnership renderLock(gGameContext->getRenderer());

		Handle hResult = BufferManager::getInstance()->createBufferArray(pVertexBuffer, pIndexBuffer, nullptr);

		if (hVertexBufferData.isValid())
		{
			hVertexBufferData.release();
			dataHandle.release();
		}

		if (hIndexBufferData.isValid())
		{
			hIndexBufferData.release();
			indexDataHandle.release();
		}

		pMesh->m_bufferArray = hResult.getObject<IGPUBufferArray>();
	}

	int MeshManager::getBufferLayoutByString(const char* stringType)
	{
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_C3);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_TC2);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_TC2_SKIN);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_T3_TC2);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_T3_TC2_SKIN);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_T3_B3_TC2);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_T3_B3_TC2_SKIN);
		return -1;
	}

	ZE::Handle MeshManager::loadResource_Internal(const char* resourceFilePath, ResourceCreateSettings* settings)
	{
		Handle meshHandle("Mesh Handle", sizeof(Mesh));
		Mesh* pMesh;

		FileReader reader(resourceFilePath);

		if (!reader.isValid())
		{
			ZEWARNING("Mesh file not found : %s", resourceFilePath);
			return meshHandle;
		}
		
		char tokenBuffer[256];
		
		pMesh = new(meshHandle) Mesh();
		while (!reader.eof())
		{
			reader.readNextString(tokenBuffer);
			if (StringFunc::Compare(tokenBuffer, "vbuff") == 0)
			{
				reader.readNextString(tokenBuffer);
				String path = GetResourcePath(tokenBuffer);
				FileReader bufferFileReader(path.c_str());
				if (!bufferFileReader.isValid())
				{
					ZEINFO("BufferManager: File %s not find", path.c_str());
					return Handle();
				}
				loadBuffers(&bufferFileReader, pMesh);
			}
			else if (StringFunc::Compare(tokenBuffer, "mat") == 0)
			{
				reader.readNextString(tokenBuffer);

				Handle hMaterial = MaterialManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
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
				loadPhysicsBodySetup(&reader, pMesh);
			}
			else if (StringFunc::Compare(tokenBuffer, "skeleton") == 0)
			{
				reader.readNextString(tokenBuffer);
				Handle hSkeleton = SkeletonManager::GetInstance()->loadResource(GetResourcePath(tokenBuffer).c_str());
				if (hSkeleton.isValid())
				{
					pMesh->m_hSkeleton = hSkeleton;
				}
			}
		}

		reader.close();
		
		return meshHandle;
	}

	void MeshManager::preUnloadResource(Resource* _resource)
	{
		Mesh* pMesh = _resource->m_hActual.getObject<Mesh>();

		if (pMesh)
		{
			if (pMesh->m_hPhysicsBodySetup.isValid())
			{
				pMesh->m_hPhysicsBodySetup.release();
			}

			if (pMesh->m_bufferArray)
			{
				pMesh->m_bufferArray->release();
			}
		}
	}

}