#ifndef __ZE_BUFFER_MANAGER__
#define __ZE_BUFFER_MANAGER__

#include "ResourceManager.h"

#include "Renderer/BufferData.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/BufferLayout.h"
#include "Utils/Array.h"

namespace ZE 
{

class GameContext;

class BufferManager : public ResourceManager
{
	DEFINE_CLASS(BufferManager)

private:
	BufferManager(GameContext* _gameContext);

public:
	
	static BufferManager* getInstance();

	// Init Buffer manager instance
	static void Init(GameContext* _gameContext);

	// Destroy manager
	static void Destroy();

	// Create GPU buffer from BufferData object
	IGPUBufferData* createGPUBufferFromBuffer(BufferData* _bufferData, bool _bStatic = true, bool _manualManage = false);

	// Create constant buffer from BufferData object
	IGPUBufferData* createConstantBufferFromBuffer(BufferData* _bufferData);

	// Create constant buffer using pointer of data and its size
	IGPUBufferData* createConstantBuffer(void* data, size_t size);

	// Create buffer array from vertex buffer, index buffer, and gpu buffer
	Handle createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer);

	// ResourceManager override
	virtual Handle loadResource_Internal(const char* resourceFilePath) override;
	virtual void preUnloadResource(Resource* _resource) override;

	IGPUBufferArray* getBufferArray(UInt32 index) { return m_GPUBufferArrays[index]; }
	
private:
	int getBufferLayoutByString(const char* stringType);

private:
	static BufferManager* m_instance;

	Array<BufferData*, true> m_buffers;
	Array<IGPUBufferData*, true> m_GPUBuffers;
	Array<IGPUBufferArray*, true> m_GPUBufferArrays;
	Array<IGPUBufferData*, true> m_constantGPUBuffer;

	GameContext* m_gameContext;
};
}
#endif
