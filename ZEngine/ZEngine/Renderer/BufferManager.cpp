#include "BufferManager.h"

#include "BufferLayout.h"

namespace ZE {

	BufferManager* BufferManager::m_instance = NULL;

	BufferManager::BufferManager()
	{
	}

	ZE::BufferManager* BufferManager::getInstance()
	{
		return m_instance;
	}

	void BufferManager::Init()
	{
		m_instance = new BufferManager();
		BufferLayoutManager::Init();
		m_instance->m_bufferLayoutManager = BufferLayoutManager::getInstance();

		// Create sample vertex Color buffer
		float* vertices_color = new float[18] {
			// Positions		// Colors			
			0.5, -0.5f, 0.0f,	1.0f, 0.0f, 0.0f, // Bottom Right
			-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // Bottom Left
			0.0f, 0.5f, 0.0f,	0.0f, 0.0f, 1.0f	// Top
		};

		BufferData* bufferData = new BufferData(BufferType::VERTEX_BUFFER);
		bufferData->SetData(vertices_color, 18 * sizeof(float));
		bufferData->m_bufferLayout = BUFFER_LAYOUT_V3_C3;

		getInstance()->m_buffers.push_back(bufferData);

		getInstance()->createBufferArray(bufferData, nullptr, nullptr);

	}

	void BufferManager::Destroy()
	{
		delete m_instance;
	}

	ZE::GPUBufferData* BufferManager::createGPUBufferFromBuffer(BufferData* _bufferData)
	{
		if (!_bufferData) return nullptr;
		// #OPENGL Specific
		GPUBufferData* GPUBuffer = new GPUBufferData();
		GPUBuffer->FromBufferData(_bufferData);

		m_GPUBuffers.push_back(GPUBuffer);
		
		return GPUBuffer;
	}

	ZE::GPUBufferArray* BufferManager::createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer)
	{
		// #OPENGL Specific
		GPUBufferData* vertexBufferGPU = createGPUBufferFromBuffer(_vertexBuffer);
		GPUBufferData* indexBufferGPU = createGPUBufferFromBuffer(_indexBuffer);
		GPUBufferData* computeGPUBuffer = createGPUBufferFromBuffer(_gpuBuffer);
	
		GPUBufferArray* bufferArray = new GPUBufferArray();
		bufferArray->SetupBufferArray(vertexBufferGPU, indexBufferGPU, computeGPUBuffer);

		m_GPUBufferArrays.push_back(bufferArray);
		return bufferArray;
	}

}