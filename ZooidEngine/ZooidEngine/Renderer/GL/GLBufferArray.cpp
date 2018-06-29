#include "GLBufferArray.h"
#include "GLBufferData.h"

namespace ZE
{
	void GLBufferArray::SetupBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _computeBuffer)
	{
		GLBufferData* vertexGLBuffer = static_cast<GLBufferData*>(_vertexBuffer);
		GLBufferData* indexGLBuffer = static_cast<GLBufferData*>(_indexBuffer);
		GLBufferData* computeGLBuffer = static_cast<GLBufferData*>(_computeBuffer);
		
		if (m_VAO == 0)
			glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexGLBuffer->getBBO());
		m_dataCount = _vertexBuffer->m_dataCount;
		m_buffers.push_back(_vertexBuffer);

		if (_indexBuffer) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexGLBuffer->getBBO());
			m_dataCount = _indexBuffer->m_dataCount;
			m_buffers.push_back(_indexBuffer);
		}
		m_bUsingIndexBuffer = _indexBuffer != nullptr;

		BindLayout(_vertexBuffer->m_layout);

		glBindVertexArray(0);
	}

	void GLBufferArray::Bind()
	{
		glBindVertexArray(m_VAO);
	}

	void GLBufferArray::Unbind()
	{
		glBindVertexArray(0);
	}

	void GLBufferArray::release()
	{
		if (m_VAO > 0)
		{
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}

	void GLBufferArray::BindLayout(BufferLayout* layout)
	{
		for (int i = 0; i < layout->m_layouts.length(); i++) {
			GLenum dataType = GL_FLOAT;
			unsigned int size_data_type = 1;
			if (layout->m_layouts[i].DataType == FLOAT) {
				size_data_type = sizeof(GLfloat);
			}
			else if (layout->m_layouts[i].DataType == INTEGER)
			{
				size_data_type = sizeof(GLint);
				dataType = GL_INT;
			}

			if(dataType == GL_INT)
			{ 
				glVertexAttribIPointer(i, layout->m_layouts[i].SizePerItem, dataType, layout->m_layouts[i].Stride, (GLvoid*)(layout->m_layouts[i].Offset * size_data_type));
			}
			else
			{
				glVertexAttribPointer(i, layout->m_layouts[i].SizePerItem, dataType, GL_FALSE, layout->m_layouts[i].Stride, (GLvoid*)(layout->m_layouts[i].Offset * size_data_type));
			}
			glEnableVertexAttribArray(i);
		}
	}

}

