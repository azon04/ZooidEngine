#include "BufferLayout.h"

#include <GL/glew.h>

namespace ZE {

	void BufferLayoutManager::InitLayout()
	{
		// #TODO init layout here
	}

	void BufferLayoutManager::DestroyLayout()
	{
		// #TODO destroy layout
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