#ifndef __ZE_BUFFER_MANAGER__
#define __ZE_BUFFER_MANAGER__

#include "ResourceManager.h"

#include "Renderer/BufferData.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/BufferLayout.h"
#include "Utils/Array.h"

// Some Util Buffer Array
#define BUFFER_ARRAY_QUAD_V3_TC2 0
#define BUFFER_ARRAY_CUBE 0

// Constant Buffers Binding Locations
#define CONSTANT_BUFFER_SHADER_DATA_INDEX 0
#define CONSTANT_BUFFER_LIGHT_DATA_INDEX 1
#define CONSTANT_BUFFER_SKELETON_DATA_INDEX 2


namespace ZE 
{

class GameContext;

class BufferManager
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
	IGPUBufferData* createGPUBufferFromBuffer(BufferData* _bufferData, bool _manualManage = false);

	// Create constant buffer from BufferData object
	IGPUBufferData* createConstantBufferFromBuffer(BufferData* _bufferData, UInt32 bindingIndex);

	// Create constant buffer using pointer of data and its size
	IGPUBufferData* createConstantBuffer(void* data, size_t size, UInt32 bindingIndex);

	// Create buffer array from vertex buffer, index buffer, and gpu buffer
	Handle createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer);

	Handle createBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _buffer);

	IGPUBufferArray* getBufferArray(UInt32 index) { return m_GPUBufferArrays[index]; }

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
