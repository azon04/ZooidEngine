#include "GLBufferArray.h"
#include "GLBufferData.h"
#include "Renderer/BufferLayout.h"

namespace ZE
{
	void GLBufferArray::SetupBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _computeBuffer)
	{
		GLBufferData* vertexGLBuffer = static_cast<GLBufferData*>(_vertexBuffer);
		GLBufferData* indexGLBuffer = static_cast<GLBufferData*>(_indexBuffer);
		GLBufferData* computeGLBuffer = static_cast<GLBufferData*>(_computeBuffer);
		
		if (m_VAO == 0)
			glGenVertexArrays(1, &m_VAO);

		// Clear current buffer
		m_buffers.clear();

		glBindVertexArray(m_VAO);
		glBindBuffer(GL_ARRAY_BUFFER, vertexGLBuffer->getBBO());
		m_dataCount = _vertexBuffer->getDataCount();
		m_buffers.push_back(_vertexBuffer);
		
		m_vertexLayoutSize = _vertexBuffer->getBufferLayout()->m_layouts.length();

		BindLayout(_vertexBuffer->getBufferLayout());

		if (_indexBuffer) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexGLBuffer->getBBO());
			m_dataCount = _indexBuffer->getDataCount();
			m_buffers.push_back(_indexBuffer);
		}
		m_indexBuffer = _indexBuffer;


		glBindVertexArray(0);
	}

	void GLBufferArray::SetupBufferArray(IGPUBufferData** _vertexBuffers, UInt32 _vertexBufferCount, IGPUBufferData** _indexBuffers, UInt32 _indexBufferCount, IGPUBufferData** _computeBuffers /*= nullptr*/, UInt32 _computeBufferCount /*= 0*/)
	{
		if (m_VAO == 0)
			glGenVertexArrays(1, &m_VAO);

		glBindVertexArray(m_VAO);

		m_vertexLayoutSize = 0;
		m_buffers.clear();
		for (UInt32 i = 0; i < _vertexBufferCount; i++)
		{
			GLBufferData* vertexBuffer = static_cast<GLBufferData*>(_vertexBuffers[i]);
			glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer->getBBO());
			
			BindLayout(vertexBuffer->getBufferLayout(), m_vertexLayoutSize);

			m_vertexLayoutSize += vertexBuffer->getBufferLayout()->m_layouts.length();

			m_buffers.push_back(vertexBuffer);
		}


		for (UInt32 i = 0; i < _indexBufferCount; i++)
		{
			GLBufferData* indexBuffer = static_cast<GLBufferData*>(_indexBuffers[i]);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer->getBBO());
			m_buffers.push_back(indexBuffer);
		}

		m_indexBuffer = _indexBufferCount > 0 ? m_buffers[0] : nullptr;

		glBindVertexArray(0);
	}

	void GLBufferArray::AddVertexBuffer(IGPUBufferData* _vertexBuffer)
	{
		glBindVertexArray(m_VAO);
		GLBufferData* vertexGLBuffer = static_cast<GLBufferData*>(_vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexGLBuffer->getBBO());
		BindLayout(vertexGLBuffer->getBufferLayout(), m_vertexLayoutSize);
		m_vertexLayoutSize += vertexGLBuffer->getBufferLayout()->m_layouts.length();
		glBindVertexArray(0);
	}

	void GLBufferArray::AddIndexBuffer(IGPUBufferData* _indexBuffer)
	{
		glBindVertexArray(m_VAO);
		GLBufferData* indexGLBuffer = static_cast<GLBufferData*>(_indexBuffer);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexGLBuffer->getBBO());
		glBindVertexArray(0);

		if (!m_indexBuffer)
		{
			m_indexBuffer = indexGLBuffer;
		}
	}

	void GLBufferArray::AddComputeBuffer(IGPUBufferData* _computeBuffer)
	{

	}

	void GLBufferArray::bind()
	{
		glBindVertexArray(m_VAO);
	}

	void GLBufferArray::unbind()
	{
		glBindVertexArray(0);
	}

	void GLBufferArray::release()
	{
		IGPUBufferArray::release();
		if (m_VAO > 0)
		{
			glDeleteVertexArrays(1, &m_VAO);
			m_VAO = 0;
		}
	}

	void GLBufferArray::BindLayout(BufferLayout* layout, UInt32 offset)
	{
		for (int i = 0; i < layout->m_layouts.length(); i++) 
		{
			Layout& currentLayout = layout->m_layouts[i];
			UInt32 index = currentLayout.Index + offset;
			GLenum dataType = GL_FLOAT;
			unsigned int size_data_type = 1;
			if (currentLayout.DataType == FLOAT) 
			{
				size_data_type = sizeof(GLfloat);
			}
			else if (currentLayout.DataType == INTEGER)
			{
				size_data_type = sizeof(GLint);
				dataType = GL_INT;
			}

			if(dataType == GL_INT)
			{ 
				glVertexAttribIPointer(index, currentLayout.SizePerItem, dataType, currentLayout.Stride, (GLvoid*)(currentLayout.Offset * size_data_type));
			}
			else
			{
				glVertexAttribPointer(index, currentLayout.SizePerItem, dataType, GL_FALSE, currentLayout.Stride, (GLvoid*)(currentLayout.Offset * size_data_type));
			}

			glEnableVertexAttribArray(index);

			if (currentLayout.bInstanceData)
			{
				glVertexAttribDivisor(index, 1);
			}
		}
	}

}

