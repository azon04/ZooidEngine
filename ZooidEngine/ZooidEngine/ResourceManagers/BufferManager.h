#ifndef __ZE_BUFFER_MANAGER__
#define __ZE_BUFFER_MANAGER__

#include "ResourceManager.h"

#include "Renderer/BufferData.h"
#include "Renderer/GPUBufferData.h"
#include "Renderer/GPUBufferArray.h"
#include "Renderer/BufferLayout.h"
#include "Utils/Array.h"

namespace ZE {

class GameContext;

class BufferManager : public ResourceManager
{
	DEFINE_CLASS(BufferManager)

private:

	static BufferManager* m_instance;

	BufferManager();

public:
	
	static BufferManager* getInstance();

	static void Init(GameContext* _gameContext);
	static void Destroy();

	GPUBufferData* createGPUBufferFromBuffer(BufferData* _bufferData, bool _bStatic = true, bool _manualManage = false);
	GPUBufferData* createConstantBufferFromBuffer(BufferData* _bufferData);
	GPUBufferData* createConstantBuffer(void* data, size_t size);
	Handle createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer);

	virtual Handle loadResource_Internal(const char* resourceFilePath) override;
	virtual void preUnloadResource(Resource* _resource) override;


	Array<BufferData*, true> m_buffers;
	Array<GPUBufferData*, true> m_GPUBuffers;
	Array<GPUBufferArray*, true> m_GPUBufferArrays;
	Array<GPUBufferData*, true> m_constantGPUBuffer;
	
private:
	int getBufferLayoutByString(const char* stringType);
};
}
#endif
