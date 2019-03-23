#include "GLBufferData.h"
#include "Renderer/BufferData.h"
#include "Memory/MemoryHelper.h"

namespace ZE
{

	void GLBufferData::FromBufferData(BufferData* _bufferData)
	{
		IGPUBufferData::FromBufferData(_bufferData);

		m_isStatic = _bufferData->isStaticBuffer();

		GLenum usage = m_isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
		if (m_BBO == 0)
		{
			glGenBuffers(1, &m_BBO);
		}
		
		if (m_bufferType == VERTEX_BUFFER) 
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			glBufferData(GL_ARRAY_BUFFER, _bufferData->getSize(), _bufferData->getData(), usage);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == INDEX_BUFFER) 
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _bufferData->getSize(), _bufferData->getData(), usage);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, m_BBO);
			glBufferData(GL_UNIFORM_BUFFER, _bufferData->getSize(), _bufferData->getData(), usage);
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}

		m_currentSize = _bufferData->getSize();
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
			if (!m_isStatic)
			{
				glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
				if (m_currentSize >= m_BufferData->getSize())
				{
					glBufferSubData(GL_ARRAY_BUFFER, 0, m_BufferData->getSize(), m_BufferData->getData());
				}
				else
				{
					glBufferData(GL_ARRAY_BUFFER, m_BufferData->getSize(), m_BufferData->getData(), GL_DYNAMIC_DRAW);
					m_currentSize = m_BufferData->getSize();
				}
				glBindBuffer(GL_ARRAY_BUFFER, 0);
			}
		}
		else if (m_bufferType == INDEX_BUFFER)
		{
			if (!m_isStatic)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
				if (m_currentSize >= m_BufferData->getSize())
				{
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_BufferData->getSize(), m_BufferData->getData());
				}
				else
				{
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_BufferData->getSize(), m_BufferData->getData(), GL_DYNAMIC_DRAW);
					m_currentSize = m_BufferData->getSize();
				}
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			if (!m_isStatic)
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, m_BBO);
				if (m_currentSize >= m_BufferData->getSize())
				{
					glBufferSubData(GL_UNIFORM_BUFFER, 0, m_BufferData->getSize(), m_BufferData->getData());
				}
				else
				{
					glBufferData(GL_UNIFORM_BUFFER, m_BufferData->getSize(), m_BufferData->getData(), GL_DYNAMIC_DRAW);
					m_currentSize = m_BufferData->getSize();
				}
				glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, 0);
			}
		}
	}

	void GLBufferData::bindAndRefresh()
	{
		if (m_bufferType == VERTEX_BUFFER)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				if (m_currentSize >= m_BufferData->getSize())
				{
					glBufferSubData(GL_ARRAY_BUFFER, 0, m_BufferData->getSize(), m_BufferData->getData());
				}
				else
				{
					glBufferData(GL_ARRAY_BUFFER, m_BufferData->getSize(), m_BufferData->getData(), GL_DYNAMIC_DRAW);
					m_currentSize = m_BufferData->getSize();
				}
			}
		}
		else if (m_bufferType == INDEX_BUFFER)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				if (m_currentSize >= m_BufferData->getSize())
				{
					glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_BufferData->getSize(), m_BufferData->getData());
				}
				else
				{
					glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_BufferData->getSize(), m_BufferData->getData(), GL_DYNAMIC_DRAW);
					m_currentSize = m_BufferData->getSize();
				}
			}
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, m_BBO);
			if (!m_isStatic)
			{
				if (m_currentSize >= m_BufferData->getSize())
				{
					glBufferSubData(GL_UNIFORM_BUFFER, 0, m_BufferData->getSize(), m_BufferData->getData());
				}
				else
				{
					glBufferData(GL_UNIFORM_BUFFER, m_BufferData->getSize(), m_BufferData->getData(), GL_DYNAMIC_DRAW);
					m_currentSize = m_BufferData->getSize();
				}
			}
		}
	}

	void GLBufferData::writeData(void* data, size_t size, size_t offset)
	{
		if (m_bufferType == VERTEX_BUFFER)
		{
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
			}
		}
		else if (m_bufferType == INDEX_BUFFER)
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, offset, size, data);
			}
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBufferBase(GL_UNIFORM_BUFFER, m_bindingIndex, m_BBO);
			if (!m_isStatic)
			{
				glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
			}
		}
	}

}