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
	
	// Setup Buffer Array from multiple vertex buffers, multiple index bufers and multiple compute buffers  (optional)
	virtual void SetupBufferArray(IGPUBufferData** _vertexBuffers, UInt32 _vertexBufferCount, IGPUBufferData** _indexBuffers, UInt32 _indexBufferCount, IGPUBufferData** _computeBuffers = nullptr, UInt32 _computeBufferCount = 0) = 0;

	// Add Additional Vertex Buffer
	virtual void AddVertexBuffer(IGPUBufferData* _vertexBuffer) = 0;

	// Add Additional Index Buffer
	virtual void AddIndexBuffer(IGPUBufferData* _indexBuffer) = 0;

	// Add Additional Compute Buffer
	virtual void AddComputeBuffer(IGPUBufferData* _computeBuffer) = 0;

	// Bind buffer array to rendering pipeline
	virtual void bind() = 0;

	// Unbind buffer array from rendering pipeline
	virtual void unbind() = 0;

	// Release buffer array
	virtual void release() {}

	// Set the render topology
	void setRenderTopology(ERenderTopologyEnum renderTopology) { m_drawTopology = renderTopology; }

	// Buffer count
	FORCEINLINE Int32 getBufferCount() const { return m_buffers.length(); }

	// Get buffer data
	IGPUBufferData* getBufferAtIndex(Int32 _index) { return m_buffers[_index]; }

	// Get Buffer data count; number of data in the buffer array
	FORCEINLINE Int32 getDataCount() const { return m_dataCount; }

	FORCEINLINE bool isUsingIndexBuffer() const { return m_indexBuffer != nullptr; }

	FORCEINLINE IGPUBufferData* getIndexBuffer() { return m_indexBuffer; }

	FORCEINLINE ERenderTopologyEnum getRenderTopology() const { return m_drawTopology; }

protected:
	
	Array<IGPUBufferData*, true> m_buffers;
	IGPUBufferData* m_indexBuffer;
	UInt32 m_dataCount;
	ERenderTopologyEnum m_drawTopology = ERenderTopologyEnum::TOPOLOGY_TRIANGLE;
};
}
#endif
