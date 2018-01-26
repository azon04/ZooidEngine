#ifndef __Z_GPU_BUFFER_ARRAY__
#define __Z_GPU_BUFFER_ARRAY__

#include "IGPUBufferArray.h"
#include "IGPUBufferData.h"

#include "Utils/Array.h"

namespace ZE {

class IGPUBufferArray {
	
public:
	IGPUBufferArray();
	virtual ~IGPUBufferArray();

	virtual void SetupBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _computeBuffer) = 0;
	
	virtual void Bind() = 0;
	virtual void Unbind() = 0;
	virtual void release() {};

	Array<IGPUBufferData*, true> m_buffers;
	
	bool m_bUsingIndexBuffer;
	UInt32 m_dataCount;
};
}
#endif
