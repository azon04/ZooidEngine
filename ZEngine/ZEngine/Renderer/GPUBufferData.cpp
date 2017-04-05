#include "GPUBufferData.h"

#include <GL/glew.h>

namespace ZE {

	void GPUBufferData::FromBufferData(BufferData* _bufferData)
	{
		m_bufferType = _bufferData->m_type;
		// #OPENGL SPECIFIC
		glGenBuffers(1, &m_BBO);
		if (m_bufferType == VERTEX_BUFFER) {
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
			glBufferData(GL_ARRAY_BUFFER, _bufferData->m_size, _bufferData->m_data, GL_STATIC_DRAW);
		}
		else if (m_bufferType == INDEX_BUFFER) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, _bufferData->m_size, _bufferData->m_data, GL_STATIC_DRAW);
		}

		m_BufferData = _bufferData;
	}

	void GPUBufferData::SetupLayout(BufferLayout* _layouts, int numberOfLayout)
	{
		this->numberOfLayout = numberOfLayout;
		this->m_layouts.reserve(numberOfLayout);
		for (int i = 0; i < numberOfLayout; i++) {
			this->m_layouts[i] = _layouts[i];
		}
	}

	void GPUBufferData::Bind()
	{
		// #OPENGL specific
		if (m_bufferType == VERTEX_BUFFER) {
			glBindBuffer(GL_ARRAY_BUFFER, m_BBO);
		}
		else if (m_bufferType == INDEX_BUFFER) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BBO);
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
	}

}
