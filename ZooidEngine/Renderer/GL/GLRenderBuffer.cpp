#include "GLRenderBuffer.h"
#include "Enums.h"

namespace ZE
{

	GLRenderBuffer::GLRenderBuffer()
		: m_rbo(0)
	{

	}

	void GLRenderBuffer::create(ERenderBufferFormat format, UInt32 width, UInt32 height)
	{
		if (m_rbo == 0)
		{
			glGenRenderbuffers(1, &m_rbo);
			glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
			glRenderbufferStorage(GL_RENDERBUFFER, getGLStorageFormat(format), width, height);
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

	void GLRenderBuffer::release()
	{
		IGPURenderBuffer::release();
		if (m_rbo > 0)
		{
			glDeleteRenderbuffers(1, &m_rbo);
			m_rbo = 0;
		}
	}

	void GLRenderBuffer::bind()
	{
		if (m_rbo > 0)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
		}
	}

	void GLRenderBuffer::unbind()
	{
		if (m_rbo > 0)
		{
			glBindRenderbuffer(GL_RENDERBUFFER, 0);
		}
	}

}