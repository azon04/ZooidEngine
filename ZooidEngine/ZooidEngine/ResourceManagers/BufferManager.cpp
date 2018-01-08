#include "../ZEngine.h"

#include "BufferManager.h"
#include "Renderer/BufferLayout.h"

#include "FileSystem/FileReader.h"

namespace ZE {

	IMPLEMENT_CLASS_1(BufferManager, ResourceManager)

	BufferManager* BufferManager::m_instance = NULL;

	BufferManager::BufferManager()
	{
	}

	ZE::BufferManager* BufferManager::getInstance()
	{
		return m_instance;
	}

	void BufferManager::Init(GameContext* _gameContext)
	{
		Handle hBufferManager("Buffer Manager", sizeof(BufferManager));
		m_instance = new(hBufferManager) BufferManager();

		BufferLayoutManager::Init();

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
			bufferData->SetData(vertices_color, 18 * sizeof(float));
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
			bufferData->SetData(cube_vertices, 288 * sizeof(float));
			bufferData->m_bufferLayout = BUFFER_LAYOUT_V3_N3_TC2;

			getInstance()->createBufferArray(bufferData, nullptr, nullptr);

			// #TODO do we really need BufferData to be saved?
			hBufferData.release();
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
			bufferData->SetData(basis_data, 36 * sizeof(float));
			bufferData->m_bufferLayout = BUFFER_LAYOUT_V3_C3;

			getInstance()->createBufferArray(bufferData, nullptr, nullptr);

			// #TODO do we really need BufferData to be saved?
			hBufferData.release();
		}

		getInstance()->loadResource("../Resources/VertexBuffers/Cube.vbuff");
	}

	void BufferManager::Destroy()
	{
		getInstance()->unloadResource("../Resources/VertexBuffers/Cube.vbuff");

		getInstance()->unloadResources();
	}

	ZE::GPUBufferData* BufferManager::createGPUBufferFromBuffer(BufferData* _bufferData, bool _bStatic, bool _manualManage)
	{
		if (!_bufferData) return nullptr;
		Handle handle("GPU Buffer Data", sizeof(GPUBufferData));
		GPUBufferData* GPUBuffer = new(handle) GPUBufferData(_bStatic);
		GPUBuffer->FromBufferData(_bufferData);

		if (!_manualManage)
		{
			m_GPUBuffers.push_back(GPUBuffer);
		}

		return GPUBuffer;
	}

	ZE::GPUBufferData* BufferManager::createConstantBufferFromBuffer(BufferData* _bufferData)
	{
		GPUBufferData* bufferData = createGPUBufferFromBuffer(_bufferData, false, true);
		bufferData->m_bindingIndex = m_constantGPUBuffer.length();
		m_constantGPUBuffer.push_back(bufferData);
		return bufferData;
	}

	ZE::GPUBufferData* BufferManager::createConstantBuffer(void* data, size_t size)
	{
		Handle hBufferData("BasisBufferData", sizeof(BufferData));
		BufferData* bufferData = new(hBufferData) BufferData(BufferType::UNIFORM_BUFFER);
		bufferData->SetData(data, size);

		// Buffer Data need to be saved since the data will be changed eventually
		m_buffers.push_back(bufferData);

		return createConstantBufferFromBuffer(bufferData);
	}

	Handle BufferManager::createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer)
	{
		GPUBufferData* vertexBufferGPU = createGPUBufferFromBuffer(_vertexBuffer);
		GPUBufferData* indexBufferGPU = createGPUBufferFromBuffer(_indexBuffer);
		GPUBufferData* computeGPUBuffer = createGPUBufferFromBuffer(_gpuBuffer);
	
		Handle handle("GPU Buffer Array", sizeof(GPUBufferArray));
		GPUBufferArray* bufferArray = new(handle) GPUBufferArray();
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

		int dataPerVertex = 6;
		switch (bufferLayoutType)
		{
		case BUFFER_LAYOUT_V3_C3:
			dataPerVertex = 6;
			break;
		case BUFFER_LAYOUT_V3_N3_TC2:
			dataPerVertex = 8;
			break;
		default:
			break;
		}

		int numVertex = fileReader.readNextInt();
		int totalSize = dataPerVertex * numVertex;
		
		Handle dataHandle("Data", sizeof(Float32) * totalSize);
		Float32* pData = new(dataHandle) Float32[totalSize];

		for (int i = 0; i < totalSize; ++i)
		{
			pData[i] = fileReader.readNextFloat();
		}

		Handle hVertexBufferData("Buffer Data", sizeof(BufferData));
		BufferData* pVertexBuffer = new(hVertexBufferData) BufferData(VERTEX_BUFFER);
		pVertexBuffer->SetData(pData, sizeof(Float32) * totalSize);

		// READ OPTIONAL INDEX BUFFER
		Handle hIndexBufferData("Buffer Data", sizeof(BufferData));
		BufferData* pIndexBuffer = nullptr;
		Handle indexDataHandle;

		fileReader.readNextString(buffer);
		if (StringFunc::Compare(buffer, "INDICE_BUFFER_NONE") != 0)
		{
			int numIndex = fileReader.readNextInt();
			indexDataHandle = Handle("Data", sizeof(Int32) * numIndex);
			Int32* indexData = new(indexDataHandle) Int32[numIndex];

			for (int i = 0; i < numIndex; ++i)
			{
				indexData[i] = fileReader.readNextInt();
			}

			pIndexBuffer = new(hIndexBufferData) BufferData(INDEX_BUFFER);
			pIndexBuffer->SetData(indexData, sizeof(Int32) * numIndex);
		}

		fileReader.close();

		Handle hResult = createBufferArray(pVertexBuffer, pIndexBuffer, nullptr);

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

	}

	int BufferManager::getBufferLayoutByString(const char* stringType)
	{
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_C3);
		COMPARE_RETURN(stringType, BUFFER_LAYOUT_V3_N3_TC2);
		return -1;
	}

}