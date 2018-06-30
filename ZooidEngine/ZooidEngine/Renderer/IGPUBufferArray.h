#ifndef __Z_GPU_BUFFER_ARRAY__
#define __Z_GPU_BUFFER_ARRAY__

#include "IGPUBufferArray.h"
#include "IGPUBufferData.h"

#include "Utils/Array.h"

namespace ZE 
{

class IGPUBufferArray 
{
	
public:
	IGPUBufferArray();
	virtual ~IGPUBufferArray();

	// Setup Buffer Array from vertex buffer, index buffer and compute buffer (optional)
	virtual void SetupBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _computeBuffer) = 0;
	
	// Bind buffer array to rendering pipeline
	virtual void bind() = 0;

	// Unbind buffer array from rendering pipeline
	virtual void unbind() = 0;

	// Release buffer array
	virtual void release() {}

	// Buffer count
	FORCEINLINE Int32 getBufferCount() const { return m_buffers.length(); }

	// Get buffer data
	IGPUBufferData* getBufferAtIndex(Int32 _index) { return m_buffers[_index]; }

	// Get Buffer data count; number of data in the buffer array
	FORCEINLINE Int32 getDataCount() const { return m_dataCount; }

	FORCEINLINE bool isUsingIndexBuffer() const { return m_bUsingIndexBuffer; }

protected:
	
	Array<IGPUBufferData*, true> m_buffers;
	bool m_bUsingIndexBuffer;
	UInt32 m_dataCount;
};
}
#endif
