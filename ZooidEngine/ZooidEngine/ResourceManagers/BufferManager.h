#ifndef __ZE_BUFFER_MANAGER__
#define __ZE_BUFFER_MANAGER__

#include "ResourceManager.h"

#include "Renderer/BufferData.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/BufferLayout.h"
#include "Utils/Array.h"

namespace ZE {

class GameContext;

class BufferManager : public ResourceManager
{
	DEFINE_CLASS(BufferManager)

private:

	static BufferManager* m_instance;

	BufferManager(GameContext* _gameContext);

public:
	
	static BufferManager* getInstance();

	static void Init(GameContext* _gameContext);
	static void Destroy();

	IGPUBufferData* createGPUBufferFromBuffer(BufferData* _bufferData, bool _bStatic = true, bool _manualManage = false);
	IGPUBufferData* createConstantBufferFromBuffer(BufferData* _bufferData);
	IGPUBufferData* createConstantBuffer(void* data, size_t size);
	Handle createBufferArray(BufferData* _vertexBuffer, BufferData* _indexBuffer, BufferData* _gpuBuffer);

	virtual Handle loadResource_Internal(const char* resourceFilePath) override;
	virtual void preUnloadResource(Resource* _resource) override;


	Array<BufferData*, true> m_buffers;
	Array<IGPUBufferData*, true> m_GPUBuffers;
	Array<IGPUBufferArray*, true> m_GPUBufferArrays;
	Array<IGPUBufferData*, true> m_constantGPUBuffer;
	
	GameContext* m_gameContext;

private:
	int getBufferLayoutByString(const char* stringType);
};
}
#endif
