#include "GPUBufferData.h"

#include <GL/glew.h>

namespace ZE {

	GPUBufferData::~GPUBufferData()
	{
		release();
	}

	void GPUBufferData::FromBufferData(BufferData* _bufferData)
	{
		m_bufferType = _bufferData->m_type;
		// #OPENGL SPECIFIC
		GLenum usage = m_isStatic ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW;
		glGenBuffers(1, &m_BBO);
		if (m_bufferType == VERTEX_BUFFER) {
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			glBufferData(GL_ARRAY_BUFFER, _bufferData->m_size, _bufferData->m_data, usage);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == INDEX_BUFFER) {
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

		m_BufferData = _bufferData;

		if (m_bufferType != UNIFORM_BUFFER)
		{
			SetupLayout(BufferLayoutManager::getInstance()->m_bufferLayout[_bufferData->m_bufferLayout]);
		}
	}

	void GPUBufferData::SetupLayout(BufferLayout* _layouts)
	{
		this->m_layout = _layouts;
	}

	void GPUBufferData::Bind()
	{
		// #OPENGL specific
		if (m_bufferType == VERTEX_BUFFER) {
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			if (!m_isStatic)
			{
				GLvoid* p = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
				MemoryHelper::Copy(m_BufferData->m_data, p, m_BufferData->m_size);
				glUnmapBuffer(GL_ARRAY_BUFFER);
			}
		}
		else if (m_bufferType == INDEX_BUFFER) {
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

	void GPUBufferData::UnBind()
	{
		// #OPENGL specific
		if (m_bufferType == VERTEX_BUFFER) {
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == INDEX_BUFFER) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
		else if (m_bufferType == UNIFORM_BUFFER)
		{
			glBindBuffer(GL_UNIFORM_BUFFER, 0);
		}
	}

	void GPUBufferData::release()
	{
		// #OPENGL Specific
		if (m_BBO)
		{
			glDeleteBuffers(1, &m_BBO);
			m_BBO = 0;
		}
	}

}
