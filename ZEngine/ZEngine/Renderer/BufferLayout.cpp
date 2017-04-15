#include "BufferLayout.h"

#include <GL/glew.h>

namespace ZE {

	void BufferLayoutManager::InitLayout()
	{
		// Vertex with color
		BufferLayout* vertexColorLayout = new BufferLayout();
		vertexColorLayout->m_layouts = {
			{ 0, 3, DataType::FLOAT, 5 * sizeof(float), 0 },
			{ 1, 2, DataType::FLOAT, 5 * sizeof(float), 3 }
		};
		m_bufferLayout.push_back(vertexColorLayout);
	}

	void BufferLayoutManager::DestroyLayout()
	{
		for (auto layout : m_bufferLayout) {
			delete layout;
		}

		m_bufferLayout.clear();
	}

	ZE::BufferLayout* BufferLayoutManager::getBufferLayoutByFormat(int format)
	{
		return m_bufferLayout[format];
	}

	void BufferLayout::BindLayout()
	{
		// #OPENGL Specific
		for (int i = 0; i < m_layouts.size(); i++) {
			GLenum dataType = GL_FLOAT;
			unsigned int size_data_type = 1;
			if (m_layouts[i].dataType == FLOAT) {
				size_data_type = sizeof(GLfloat);
			}

			glVertexAttribPointer(i, m_layouts[i].sizePerItem, dataType, GL_FALSE, m_layouts[i].stride, (GLvoid*)(m_layouts[i].offset * size_data_type));
			glEnableVertexAttribArray(i);
		}
	}

}