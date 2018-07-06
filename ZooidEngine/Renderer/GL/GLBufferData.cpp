#include "GLBufferData.h"

namespace ZE
{

	void GLBufferData::FromBufferData(BufferData* _bufferData)
	{
		IGPUBufferData::FromBufferData(_bufferData);

		GLenum usage = m_isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
		if (m_BBO == 0)
		{
			glGenBuffers(1, &m_BBO);
		}
		
		if (m_bufferType == VERTEX_BUFFER) 
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			glBufferData(GL_ARRAY_BUFFER, _bufferData->m_size, _bufferData->m_data, usage);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == INDEX_BUFFER) 
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _bufferData->m_size, _bufferData->m_data, usage);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, m_BBO);
			glBufferData(GL_UNIFORM_BUFFER, _bufferData->m_size, _bufferData->m_data, usage);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

	}

	void GLBufferData::bind()
	{
		if (m_bufferType == VERTEX_BUFFER)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
		}
		else if (m_bufferType == INDEX_BUFFER) 
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, m_BBO);
		}
	}

	void GLBufferData::unbind()
	{
		if (m_bufferType == VERTEX_BUFFER) 
		{
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == INDEX_BUFFER) 
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}

	void GLBufferData::release()
	{
		if (m_BBO)
		{
			glDeleteBuffers(1, &m_BBO);
			m_BBO = 0;
		}
	}

	void GLBufferData::refresh()
	{
		if (m_bufferType == VERTEX_BUFFER)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				GLvoid* p = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				MemoryHelper::Copy(m_BufferData->m_data, p, m_BufferData->m_size);
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == INDEX_BUFFER)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				GLvoid* p = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
				MemoryHelper::Copy(m_BufferData->m_data, p, m_BufferData->m_size);
				glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, m_BBO);
			if (!m_isStatic)
			{
				GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				MemoryHelper::Copy(m_BufferData->m_data, p, m_BufferData->m_size);
				glUnmapBuffer(GL_UNIFORM_BUFFER);
			}
			glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, 0);
		}
	}

	void GLBufferData::bindAndRefresh()
	{
		if (m_bufferType == VERTEX_BUFFER)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				GLvoid* p = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				MemoryHelper::Copy(m_BufferData->m_data, p, m_BufferData->m_size);
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}
		}
		else if (m_bufferType == INDEX_BUFFER)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				GLvoid* p = glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY);
				MemoryHelper::Copy(m_BufferData->m_data, p, m_BufferData->m_size);
				glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
			}
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, m_BBO);
			if (!m_isStatic)
			{
				GLvoid* p = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				MemoryHelper::Copy(m_BufferData->m_data, p, m_BufferData->m_size);
				glUnmapBuffer(GL_UNIFORM_BUFFER);
			}
		}
	}

}