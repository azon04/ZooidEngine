#ifndef __ZE_BUFFER_MANAGER__
#define __ZE_BUFFER_MANAGER__

#include "BufferData.h"
#include "GPUBufferData.h"
#include "GPUBufferArray.h"
#include "BufferLayout.h"
#include <vector>

namespace ZE {
class BufferManager {
	
	static BufferManager* m_instance;
public:
	
	BufferManager* getInstance();

	void Init();
	void Destroy();

	GPUBufferData* createGPUBufferFromBuffer(BufferData* _bufferData);
	GPUBufferArray* createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer);

	std::vector<BufferData*> m_buffers;
	std::vector<GPUBufferData*> m_GPUBuffers;
};
}
#endif
