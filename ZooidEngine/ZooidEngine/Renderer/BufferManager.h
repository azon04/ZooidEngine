#ifndef __ZE_BUFFER_MANAGER__
#define __ZE_BUFFER_MANAGER__

#include "BufferData.h"
#include "GPUBufferData.h"
#include "GPUBufferArray.h"
#include "BufferLayout.h"
#include "GameObjectModel/Component.h"
#include "Utils/Array.h"

namespace ZE {
class BufferManager {

	static BufferManager* m_instance;

	BufferManager();

public:
	
	static BufferManager* getInstance();

	static void Init(GameContext* _gameContext);
	static void Destroy();

	GPUBufferData* createGPUBufferFromBuffer(BufferData* _bufferData, bool _bStatic = true, bool _manualManage = false);
	GPUBufferData* createConstantBufferFromBuffer(BufferData* _bufferData);
	GPUBufferData* createConstantBuffer(void* data, size_t size);
	GPUBufferArray* createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer);

	Array<BufferData*, true> m_buffers;
	Array<GPUBufferData*, true> m_GPUBuffers;
	Array<GPUBufferArray*, true> m_GPUBufferArrays;
	Array<GPUBufferData*, true> m_constantGPUBuffer;
};
}
#endif
