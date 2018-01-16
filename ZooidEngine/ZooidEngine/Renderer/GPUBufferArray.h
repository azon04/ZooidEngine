#ifndef __Z_GPU_BUFFER_ARRAY__
#define __Z_GPU_BUFFER_ARRAY__

#include "GPUBufferArray.h"
#include "GPUBufferData.h"

#include "../Utils/Array.h"

namespace ZE {

class GPUBufferArray {
	
public:
	GPUBufferArray();
	virtual ~GPUBufferArray();

	void SetupBufferArray(GPUBufferData* _vertexBuffer, GPUBufferData* _indexBuffer, GPUBufferData* _computeBuffer);
	
	void Bind();
	void Unbind();
	void release();

	Array<GPUBufferData*, true> m_buffers;
	
	bool m_bUsingIndexBuffer;
	UInt32 m_dataCount;

#if Z_RENDER_OPENGL
	GLuint m_VAO;
#endif
};
}
#endif
