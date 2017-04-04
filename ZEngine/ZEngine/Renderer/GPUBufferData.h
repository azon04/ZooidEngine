#ifndef __Z_GPU_BUFFER__
#define __Z_GPU_BUFFER__

#include "BufferData.h"

namespace ZE {
class GPUBufferData {

public:
	GPUBufferData() : m_BBO(0) {}
	virtual ~GPUBufferData() {}

	void FromBufferData(BufferData* _bufferData);

	void Bind();
	void UnBind();

	BufferType m_bufferType;
	BufferData* m_BufferData;

	// #OPENGL Specific
	unsigned int m_BBO;
};
};
#endif // __Z_GPU_BUFFER__

