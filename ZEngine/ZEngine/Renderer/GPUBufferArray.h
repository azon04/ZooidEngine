#ifndef __Z_GPU_BUFFER_ARRAY__
#define __Z_GPU_BUFFER_ARRAY__

#include "GPUBufferArray.h"
#include "GPUBufferData.h"

#include <vector>

namespace ZE {

class GPUBufferArray {
	
public:
	GPUBufferArray();
	virtual ~GPUBufferArray();

	void SetupBufferArray(GPUBufferData* _vertexBuffer, GPUBufferData* _indexBuffer, GPUBufferData* _computeBuffer);
	
	void Bind();
	void Unbind();

	std::vector<GPUBufferData*> m_buffers;
	
	bool m_bUsingIndexBuffer;

	// #OPENGL Specific
	GLuint m_VAO;
};
}
#endif
