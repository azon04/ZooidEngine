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
		m_instance->m_bufferLayoutManager = new BufferLayoutManager();
		m_instance->m_bufferLayoutManager->InitLayout();
	}

	void BufferManager::Destroy()
	{
		delete m_instance;
	}

	ZE::GPUBufferData* BufferManager::createGPUBufferFromBuffer(BufferData* _bufferData)
	{
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
		GPUBufferData* computeGPUBuffer = NULL;
		if (_gpuBuffer) {
			computeGPUBuffer = createGPUBufferFromBuffer(_gpuBuffer);
		}

		GPUBufferArray* bufferArray = new GPUBufferArray();
		bufferArray->SetupBufferArray(vertexBufferGPU, indexBufferGPU, nullptr);

		return bufferArray;
	}

}