#include "GPUBufferArray.h"

// #OPENGL Specific
#include <GL/glew.h>

namespace ZE {

	GPUBufferArray::GPUBufferArray() : m_VAO(0)
	{

	}

	GPUBufferArray::~GPUBufferArray()
	{
		release();
	}

	void GPUBufferArray::SetupBufferArray(GPUBufferData* _vertexBuffer, GPUBufferData* _indexBuffer, GPUBufferData* _computeBuffer)
	{
		// #OPENGL Specific
		if (m_VAO == 0)
			glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, _vertexBuffer->m_BBO);
		m_dataCount = _vertexBuffer->m_dataCount;

		if (_indexBuffer) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indexBuffer->m_BBO);
			m_dataCount = _vertexBuffer->m_dataCount;
		}
		m_bUsingIndexBuffer = _indexBuffer != nullptr;

		_vertexBuffer->m_layout->BindLayout();

		glBindVertexArray(0);
	}

	void GPUBufferArray::Bind()
	{
		// #OPENGL specific
		glBindVertexArray(m_VAO);
	}

	void GPUBufferArray::Unbind()
	{
		// #OPENGL specific
		glBindVertexArray(0);
	}

	void GPUBufferArray::release()
	{
		// #OPENGL Specific
		if (m_VAO > 0)
		{
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}

}