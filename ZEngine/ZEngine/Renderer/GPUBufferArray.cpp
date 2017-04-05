#include "GPUBufferArray.h"

// #OPENGL Specific
#include <GL/glew.h>

namespace ZE {

	GPUBufferArray::GPUBufferArray() : m_VAO(0)
	{

	}

	GPUBufferArray::~GPUBufferArray()
	{
		// #OPENGL Specific
		if (m_VAO > 0) {
			glDeleteVertexArrays(1, &m_VAO);
		}
	}

	void GPUBufferArray::SetupBufferArray(GPUBufferData* _vertexBuffer, GPUBufferData* _colorBuffer, GPUBufferData* _computeBuffer)
	{
		// #OPENGL Specific
		if (m_VAO == 0)
			glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer->m_BBO);

		

		glBindVertexArray(0);
	}

}