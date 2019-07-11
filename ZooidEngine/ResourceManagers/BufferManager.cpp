#include "ZEGameContext.h"
#include "Renderer/RenderZooid.h"

#include "BufferManager.h"
#include "Renderer/BufferLayout.h"

#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"

#include "Renderer/IRenderer.h"

namespace ZE 
{

	IMPLEMENT_CLASS_0(BufferManager)

	BufferManager* BufferManager::m_instance = NULL;

	BufferManager::BufferManager(GameContext* _gameContext) : m_gameContext(_gameContext), m_currentDrawBuffer(0)
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

		ScopedRenderThreadOwnership renderLock(_gameContext->getRenderer());

		// Create sample vertex Color buffer
		{
			float quadVertices[] =
			{
				-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
				1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
				-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,

				1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
				1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
				-1.0f, 1.0f, 0.0f,		0.0f, 1.0f
			};

			Handle hBufferData("Buffer Data", sizeof(BufferData));
			BufferData* bufferData = new(hBufferData) BufferData(EBufferType::VERTEX_BUFFER);
			bufferData->SetData(quadVertices, 5 * sizeof(float), 6);
			bufferData->setBufferLayout(BUFFER_LAYOUT_V3_TC2);

			getInstance()->createBufferArray(bufferData, nullptr, nullptr);
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
			BufferData* bufferData = new(hBufferData) BufferData(EBufferType::VERTEX_BUFFER);
			bufferData->SetData(cube_vertices, 8 * sizeof(float), 36);
			bufferData->setBufferLayout(BUFFER_LAYOUT_V3_N3_TC2);

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
			BufferData* bufferData = new(hBufferData) BufferData(EBufferType::VERTEX_BUFFER);
			bufferData->SetData(basis_data, 6 * sizeof(float), 6);
			bufferData->setBufferLayout(BUFFER_LAYOUT_V3_C3);

			getInstance()->createBufferArray(bufferData, nullptr, nullptr);

			// #TODO do we really need BufferData to be saved?
			hBufferData.release();
		}
	}

	void BufferManager::Destroy()
	{
		BufferManager* bufferManager = getInstance();
		for (int i = 0; i < bufferManager->m_GPUBufferArrays.size(); i++)
		{
			bufferManager->m_GPUBufferArrays[i]->release();
		}

		bufferManager->m_GPUBufferArrays.clear();

		for (int i = 0; i < bufferManager->m_GPUBuffers.size(); i++)
		{
			bufferManager->m_GPUBuffers[i]->release();
		}

		bufferManager->m_GPUBuffers.clear();
	}

	ZE::IGPUBufferData* BufferManager::createGPUBufferFromBuffer(BufferData* _bufferData, bool _manualManage)
	{
		if (!_bufferData) return nullptr;
		Handle handle = m_gameContext->getRenderZooid()->CreateRenderBufferData();
		IGPUBufferData* GPUBuffer = handle.getObject<IGPUBufferData>();
		GPUBuffer->FromBufferData(_bufferData);

		if (!_manualManage)
		{
			m_GPUBuffers.push_back(GPUBuffer);
		}

		return GPUBuffer;
	}

	ZE::IGPUBufferData* BufferManager::createConstantBufferFromBuffer(BufferData* _bufferData, UInt32 bindingIndex)
	{
		IGPUBufferData* bufferData = createGPUBufferFromBuffer(_bufferData, true);
		bufferData->m_bindingIndex = bindingIndex;
		m_constantGPUBuffer.push_back(bufferData);
		return bufferData;
	}

	ZE::IGPUBufferData* BufferManager::createConstantBuffer(void* data, size_t size, UInt32 bindingIndex, bool bStaticBuffer)
	{
		Handle hBufferData("BasisBufferData", sizeof(BufferData));
		BufferData* bufferData = new(hBufferData) BufferData(EBufferType::UNIFORM_BUFFER);
		bufferData->SetData(data, size);
		bufferData->setStaticBuffer(bStaticBuffer);

		// Buffer Data need to be saved since the data will be changed eventually
		m_buffers.push_back(bufferData);

		IGPUBufferData* res = nullptr;
		{
			ScopedRenderThreadOwnership renderLock(m_gameContext->getRenderer());

			res = createConstantBufferFromBuffer(bufferData, bindingIndex);
		}
		return res;
	}

	ZE::IGPUBufferData* BufferManager::getOrCreateDrawBuffer(void* data, size_t size)
	{
		IGPUBufferData* result = nullptr;
		if (m_currentDrawBuffer >= m_shaderDrawBuffers.size())
		{
			result = createConstantBuffer(data, size, CONSTANT_BUFFER_DRAW_DATA_INDEX);
			m_shaderDrawBuffers.push_back(result);
		}
		else
		{
			result = m_shaderDrawBuffers[m_currentDrawBuffer];
			BufferData buffData(UNIFORM_BUFFER);
			buffData.SetData(data, size);
			result->FromBufferData(&buffData);
		}

		m_currentDrawBuffer++;
		return result;
	}

	void BufferManager::setupForNextFrame()
	{
		m_currentDrawBuffer = 0;
	}

	Handle BufferManager::createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer)
	{
		IGPUBufferData* vertexBufferGPU = createGPUBufferFromBuffer(_vertexBuffer);
		IGPUBufferData* indexBufferGPU = createGPUBufferFromBuffer(_indexBuffer);
		IGPUBufferData* computeGPUBuffer = createGPUBufferFromBuffer(_gpuBuffer);
	
		return createBufferArray(vertexBufferGPU, indexBufferGPU, computeGPUBuffer);
	}

	ZE::Handle BufferManager::createBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _conputeBuffer)
	{
		Handle handle = m_gameContext->getRenderZooid()->CreateRenderBufferArray();
		IGPUBufferArray* bufferArray = handle.getObject<IGPUBufferArray>();
		bufferArray->SetupBufferArray(_vertexBuffer, _indexBuffer, _conputeBuffer);

		m_GPUBufferArrays.push_back(bufferArray);

		return handle;
	}

}