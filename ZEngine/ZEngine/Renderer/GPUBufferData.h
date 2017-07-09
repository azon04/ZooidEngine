#ifndef __Z_GPU_BUFFER__
#define __Z_GPU_BUFFER__

#include "../Utils/PrimitiveTypes.h"

#include "BufferData.h"
#include "BufferLayout.h"

#include <vector>

// #OPENGL Specific
#include <GL/glew.h>

#define MAX_LAYOUT 16
namespace ZE {

class GPUBufferData {

public:
	GPUBufferData() : m_BBO(0) {}
	virtual ~GPUBufferData() {}

	void FromBufferData(BufferData* _bufferData);
	void SetupLayout(BufferLayout* _layouts);

	void Bind();
	void UnBind();

	BufferType m_bufferType;
	BufferData* m_BufferData;

	// #OPENGL Specific
	GLuint m_BBO;
	ZE::Int32 numberOfLayout;
	BufferLayout* m_layout;

};
};
#endif // __Z_GPU_BUFFER__

