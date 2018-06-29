#include "ZEGameContext.h"
#include "Renderer/RenderZooid.h"

#include "BufferManager.h"
#include "Renderer/BufferLayout.h"

#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"

#include "Renderer/IRenderer.h"

namespace ZE 
{

	IMPLEMENT_CLASS_1(BufferManager, ResourceManager)

	struct MeshSkinData
	{
		Float32 Position[3];
		Float32 Normal[3];
		Float32 TexCoord[2];
		Int32 BoneIDs[4];
		Float32 BoneWeights[4];
	};

	BufferManager* BufferManager::m_instance = NULL;

	BufferManager::BufferManager(GameContext* _gameContext) : m_gameContext(_gameContext)
	{
	}

	ZE::BufferManager* BufferManager::getInstance()
	{
		return m_instance;
	}

	void BufferManager::Init(GameContext* _gameContext)
	{
		Handle hBufferManager("Buffer Manager", sizeof(BufferManager));
		m_instance = new(hBufferManager) BufferManager(_gameContext);

		BufferLayoutManager::Init();

		_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		// Create sample vertex Color buffer
		{
			Handle handle("Data Triangle", sizeof(float) * 18);
			handle.getObject();
			float* vertices_color = new(handle) float[18]{
				// Positions		// Colors			
				0.5, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f, // Bottom Right
				-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom Left
				0.0f, 0.5f, 0.0f,	0.0f, 0.0f, 1.0f	// Top
			};

			Handle hBufferData("Buffer Data", sizeof(BufferData));
			BufferData* bufferData = new(hBufferData) BufferData(BufferType::VERTEX_BUFFER);
			bufferData->SetData(vertices_color, 6 * sizeof(float), 3);
			bufferData->m_bufferLayout = BUFFER_LAYOUT_V3_C3;

			getInstance()->createBufferArray(bufferData, nullptr, nullptr);

			// #TODO do we really need BufferData to be saved?
			hBufferData.release();
		}

		// Create Cube
		{
			Handle handle("Cube Data", sizeof(float) * 288);
			float* cube_vertices = new(handle) float[288]
			{
				// positions			// normal				// texture coords
				-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,
				0.5f, -0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 0.0f,
				0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
				0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		1.0f, 1.0f,
				-0.5f, 0.5f, -0.5f,		0.0f, 0.0f, -1.0f,		0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,	0.0f, 0.0f, -1.0f,		0.0f, 0.0f,

				-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,
				0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 0.0f,
				0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
				0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		1.0f, 1.0f,
				-0.5f, 0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 1.0f,
				-0.5f, -0.5f, 0.5f,		0.0f, 0.0f, 1.0f,		0.0f, 0.0f,

				-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
				-0.5f, 0.5f, -0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
				-0.5f, -0.5f, -0.5f,	-1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
				-0.5f, -0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
				-0.5f, 0.5f, 0.5f,		-1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

				0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,
				0.5f, 0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 1.0f,
				0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
				0.5f, -0.5f, -0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 1.0f,
				0.5f, -0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		0.0f, 0.0f,
				0.5f, 0.5f, 0.5f,		1.0f, 0.0f, 0.0f,		1.0f, 0.0f,

				-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,
				0.5f, -0.5f, -0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 1.0f,
				0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
				0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		1.0f, 0.0f,
				-0.5f, -0.5f, 0.5f,		0.0f, -1.0f, 0.0f,		0.0f, 0.0f,
				-0.5f, -0.5f, -0.5f,	0.0f, -1.0f, 0.0f,		0.0f, 1.0f,

				-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f,
				0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 1.0f,
				0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
				0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		1.0f, 0.0f,
				-0.5f, 0.5f, 0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 0.0f,
				-0.5f, 0.5f, -0.5f,		0.0f, 1.0f, 0.0f,		0.0f, 1.0f
			};

			Handle hBufferData("CubeBuffer Data", sizeof(BufferData));
			BufferData* bufferData = new(hBufferData) BufferData(BufferType::VERTEX_BUFFER);
			bufferData->SetData(cube_vertices, 8 * sizeof(float), 36);
			bufferData->m_bufferLayout = BUFFER_LAYOUT_V3_N3_TC2;

			getInstance()->createBufferArray(bufferData, nullptr, nullptr);

			// #TODO do we really need BufferData to be saved?
			hBufferData.release();
			handle.release();
		}

		// Create Basis vertices
		{
			Handle handle("Basis Data", sizeof(float) * 36);
			float* basis_data = new(handle) float[36]
			{
				// positions		// color
				0.0f, 0.0f, 0.0f,	0.0f, 0.0f, 1.0f,
				0.0f, 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,

				0.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,
				1.0f, 0.0f, 0.0f,	1.0f, 0.0f, 0.0f,

				0.0f, 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,
				0.0f, 0.0f, 1.0f,	0.0f, 1.0f, 0.0f
			};

			Handle hBufferData("BasisBufferData", sizeof(BufferData));
			BufferData* bufferData = new(hBufferData) BufferData(BufferType::VERTEX_BUFFER);
			bufferData->SetData(basis_data, 6 * sizeof(float), 6);
			bufferData->m_bufferLayout = BUFFER_LAYOUT_V3_C3;

			getInstance()->createBufferArray(bufferData, nullptr, nullptr);

			// #TODO do we really need BufferData to be saved?
			hBufferData.release();
		}

		_gameContext->getRenderer()->ReleaseRenderThreadOwnership();

		getInstance()->loadResource(GetPackageAssetPath("Basic", "VertexBuffer", "Cube.vbuff").c_str());
	}

	void BufferManager::Destroy()
	{
		getInstance()->unloadResources();
	}

	ZE::IGPUBufferData* BufferManager::createGPUBufferFromBuffer(BufferData* _bufferData, bool _bStatic, bool _manualManage)
	{
		if (!_bufferData) return nullptr;
		Handle handle = m_gameContext->getRenderZooid()->CreateRenderBufferData();
		IGPUBufferData* GPUBuffer = handle.getObject<IGPUBufferData>();
		GPUBuffer->m_isStatic = _bStatic;
		GPUBuffer->FromBufferData(_bufferData);

		if (!_manualManage)
		{
			m_GPUBuffers.push_back(GPUBuffer);
		}

		return GPUBuffer;
	}

	ZE::IGPUBufferData* BufferManager::createConstantBufferFromBuffer(BufferData* _bufferData)
	{
		IGPUBufferData* bufferData = createGPUBufferFromBuffer(_bufferData, false, true);
		bufferData->m_bindingIndex = m_constantGPUBuffer.length();
		m_constantGPUBuffer.push_back(bufferData);
		return bufferData;
	}

	ZE::IGPUBufferData* BufferManager::createConstantBuffer(void* data, size_t size)
	{
		Handle hBufferData("BasisBufferData", sizeof(BufferData));
		BufferData* bufferData = new(hBufferData) BufferData(BufferType::UNIFORM_BUFFER);
		bufferData->SetData(data, size);

		// Buffer Data need to be saved since the data will be changed eventually
		m_buffers.push_back(bufferData);

		m_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		IGPUBufferData* res = createConstantBufferFromBuffer(bufferData);

		m_gameContext->getRenderer()->ReleaseRenderThreadOwnership();

		return res;
	}

	Handle BufferManager::createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer)
	{
		IGPUBufferData* vertexBufferGPU = createGPUBufferFromBuffer(_vertexBuffer);
		IGPUBufferData* indexBufferGPU = createGPUBufferFromBuffer(_indexBuffer);
		IGPUBufferData* computeGPUBuffer = createGPUBufferFromBuffer(_gpuBuffer);
	
		Handle handle = m_gameContext->getRenderZooid()->CreateRenderBufferArray();
		IGPUBufferArray* bufferArray = handle.getObject<IGPUBufferArray>();
		bufferArray->SetupBufferArray(vertexBufferGPU, indexBufferGPU, computeGPUBuffer);


		m_GPUBufferArrays.push_back(bufferArray);
		return handle;
	}

	ZE::Handle BufferManager::loadResource_Internal(const char* resourceFilePath)
	{
		FileReader fileReader(resourceFilePath);

		if (!fileReader.isValid())
		{
			ZEINFO("BufferManager: File %s not find", resourceFilePath);
			return Handle();
		}

		char buffer[256];

		// READ VERTEX BUFFER
		fileReader.readNextString(buffer);
		int bufferLayoutType = getBufferLayoutByString(buffer);
		if (bufferLayoutType == -1)
		{
			ZEINFO("BufferManager: Can't find buffer layout type for %s", buffer);
			return Handle();
		}

		Handle hVertexBufferData("Buffer Data", sizeof(BufferData));
		BufferData* pVertexBuffer = new(hVertexBufferData) BufferData(VERTEX_BUFFER);
		pVertexBuffer->m_bufferLayout = bufferLayoutType;

		int numVertex = fileReader.readNextInt();
		
		Handle dataHandle;

		if (bufferLayoutType == BUFFER_LAYOUT_V3_N3_TC2_SKIN)
		{
			dataHandle = Handle("Data", sizeof(MeshSkinData) * numVertex);
			MeshSkinData* pData = new(dataHandle) MeshSkinData[numVertex];

			for (int i = 0; i < numVertex; i++)
			{
				pData[i].Position[0] = fileReader.readNextFloat();
				pData[i].Position[1] = fileReader.readNextFloat();
				pData[i].Position[2] = fileReader.readNextFloat();
				pData[i].Normal[0] = fileReader.readNextFloat();
				pData[i].Normal[1] = fileReader.readNextFloat();
				pData[i].Normal[2] = fileReader.readNextFloat();
				pData[i].TexCoord[0] = fileReader.readNextFloat();
				pData[i].TexCoord[1] = fileReader.readNextFloat();
				pData[i].BoneIDs[0] = fileReader.readNextInt();
				pData[i].BoneIDs[1] = fileReader.readNextInt();
				pData[i].BoneIDs[2] = fileReader.readNextInt();
				pData[i].BoneIDs[3] = fileReader.readNextInt();
				pData[i].BoneWeights[0] = fileReader.readNextFloat();
				pData[i].BoneWeights[1] = fileReader.readNextFloat();
				pData[i].BoneWeights[2] = fileReader.readNextFloat();
				pData[i].BoneWeights[3] = fileReader.readNextFloat();
			}

			pVertexBuffer->SetData(pData, sizeof(MeshSkinData), numVertex);
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
			case BUFFER_LAYOUT_V3_N3_TC2_SKIN:
				dataPerVertex = 16;
				break;
			default:
				break;
			}

			int totalSize = dataPerVertex * numVertex;

			dataHandle = Handle("Data", sizeof(Float32) * totalSize);
			Float32* pData = new(dataHandle) Float32[totalSize];

			for (int i = 0; i < totalSize; ++i)
			{
				pData[i] = fileReader.readNextFloat();
			}
			pVertexBuffer->SetData(pData, sizeof(Float32) * dataPerVertex, numVertex);
		}
		

		// READ OPTIONAL INDEX BUFFER
		Handle hIndexBufferData("Buffer Data", sizeof(BufferData));
		BufferData* pIndexBuffer = nullptr;
		Handle indexDataHandle;

		fileReader.readNextString(buffer);
		if (StringFunc::Compare(buffer, "INDICE_BUFFER_NONE") != 0)
		{
			int numIndex = fileReader.readNextInt();
			indexDataHandle = Handle("Data", sizeof(Int32) * numIndex);
			UInt32* indexData = new(indexDataHandle) UInt32[numIndex];

			for (int i = 0; i < numIndex; ++i)
			{
				indexData[i] = fileReader.readNextInt();
			}

			pIndexBuffer = new(hIndexBufferData) BufferData(INDEX_BUFFER);
			pIndexBuffer->SetData(indexData, sizeof(UInt32), numIndex);
		}

		fileReader.close();
		
		m_gameContext->getRenderer()->AcquireRenderThreadOwnership();

		Handle hResult = createBufferArray(pVertexBuffer, pIndexBuffer, nullptr);

		m_gameContext->getRenderer()->ReleaseRenderThreadOwnership();

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

		return hResult;

	}

	void BufferManager::preUnloadResource(Resource* _resource)
	{
		IGPUBufferArray* gpuBufferArray = _resource->m_hActual.getObject<IGPUBufferArray>();
		m_GPUBufferArrays.removeAt(m_GPUBufferArrays.firstIndexOf(gpuBufferArray));
		
		if (gpuBufferArray)
		{
			gpuBufferArray->release();
			for (int i = 0; i < gpuBufferArray->m_buffers.length(); ++i)
			{
				gpuBufferArray->m_buffers[i]->release();
				m_GPUBuffers.removeAt(m_GPUBuffers.firstIndexOf(gpuBufferArray->m_buffers[i]));
			}
		}
	}

	int BufferManager::getBufferLayoutByString(const char* stringType)
	{
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_C3);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_TC2);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_TC2_SKIN);
		return -1;
	}

}