#ifndef __Z_GPU_BUFFER__
#define __Z_GPU_BUFFER__

#include "../Utils/PrimitiveTypes.h"

#include "BufferData.h"
#include "BufferLayout.h"

#if Z_RENDER_OPENGL
#include <GL/glew.h>
#endif

#define MAX_LAYOUT 16
namespace ZE {

class GPUBufferData {

public:
	GPUBufferData() 
	{
#if Z_RENDER_OPENGL
		m_BBO = 0;
#endif
		m_isStatic = true;
	}

	GPUBufferData(bool _isStaticBuffer)
		: GPUBufferData()
	{
		m_isStatic = _isStaticBuffer;
	}

	virtual ~GPUBufferData();

	void FromBufferData(BufferData* _bufferData);
	void SetupLayout(BufferLayout* _layouts);

	void Bind();
	void UnBind();
	void release();

	BufferType m_bufferType;
	BufferData* m_BufferData;

#if Z_RENDER_OPENGL
	GLuint m_BBO;
#endif

	bool m_isStatic;

	ZE::Int32 m_bindingIndex;
	BufferLayout* m_layout;

};
};
#endif // __Z_GPU_BUFFER__

