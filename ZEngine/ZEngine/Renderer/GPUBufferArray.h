#ifndef __Z_GPU_BUFFER_ARRAY__
#define __Z_GPU_BUFFER_ARRAY__

#include "GPUBufferArray.h"

#include <vector>

namespace ZE {

class GPUBufferArray {
	
public:
	GPUBufferArray();
	virtual ~GPUBufferArray();

	void SetupBufferArray(GPUBufferData* _vertexBuffer, GPUBufferData* _colorBuffer, GPUBufferData* _computeBuffer);
	
	void Bind();
	void Unbind();

	std::vector<GPUBufferData*> m_buffers;
	
	// #OPENGL Specific
	unsigned int m_VAO;
};
}
#endif
