#include "GLFrameBuffer.h"

#include "GLTexture.h"
#include "GLRenderBuffer.h"
#include "Enums.h"

namespace ZE
{

	GLFrameBuffer::GLFrameBuffer()
		: m_bColorAttached(false),
		m_fbo(0)
	{

	}

	void GLFrameBuffer::create()
	{
		IGPUFrameBuffer::create();

		glGenFramebuffers(1, &m_fbo);
	}

	void GLFrameBuffer::addTextureAttachment(EFrameBuferAttachmentType attachType, IGPUTexture* texture, UInt32 attachIndex /*= 0*/)
	{
		GLenum _glAttachType = getGLAttachmentType(attachType, attachIndex);

		GLTexture* pRealTexture = (GLTexture*)texture;
		glFramebufferTexture2D(GL_FRAMEBUFFER, _glAttachType, GL_TEXTURE_2D, pRealTexture->getTextureBuffer(), 0);

		if (attachType == COLOR_ATTACHMENT)
		{
			m_bColorAttached = true;
			if (!m_drawBuffers.contains(_glAttachType))
			{
				m_drawBuffers.push_back(_glAttachType);
			}
		}
	}

	void GLFrameBuffer::addRenderBufferAttachment(EFrameBuferAttachmentType attachType, IGPURenderBuffer* renderBuffer, UInt32 attachIndex /*= 0*/)
	{
		GLenum _glAttachType = getGLAttachmentType(attachType, attachIndex);

		GLRenderBuffer* pRealRenderBuffer = (GLRenderBuffer*)renderBuffer;
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, _glAttachType, GL_RENDERBUFFER, pRealRenderBuffer->getRenderBufferObject());

		if (attachType == COLOR_ATTACHMENT)
		{
			m_bColorAttached = true;
			if (!m_drawBuffers.contains(_glAttachType))
			{
				m_drawBuffers.push_back(_glAttachType);
			}
		}
	}

	void GLFrameBuffer::addTextureCubeAttachment(EFrameBuferAttachmentType attachType, IGPUTexture* texture, UInt32 faceIndex, UInt32 attachIndex /*= 0*/, UInt32 mipIndex)
	{
		GLenum _glAttachType = getGLAttachmentType(attachType, attachIndex);

		GLTexture* pRealTexture = (GLTexture*)texture;
		glFramebufferTexture2D(GL_FRAMEBUFFER, _glAttachType, GL_TEXTURE_CUBE_MAP_POSITIVE_X + faceIndex, pRealTexture->getTextureBuffer(), mipIndex);

		if (attachType == COLOR_ATTACHMENT)
		{
			m_bColorAttached = true;
			if (!m_drawBuffers.contains(_glAttachType))
			{
				m_drawBuffers.push_back(_glAttachType);
			}
		}
	}

	void GLFrameBuffer::setupAttachments()
	{
		if (!m_bColorAttached)
		{
			glDrawBuffer(GL_NONE);
			glReadBuffer(GL_NONE);
		}
		else if( m_drawBuffers.length() > 1)
		{
			glDrawBuffers(m_drawBuffers.length(), m_drawBuffers.data());
		}

		ZASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is not complete. Try to attach all necessasy attachments.");
	}

	void GLFrameBuffer::release()
	{
		IGPUFrameBuffer::release();
		if (m_fbo > 0)
		{
			glDeleteFramebuffers(1, &m_fbo);
			m_fbo = 0;
		}
	}

	void GLFrameBuffer::bind()
	{
		if (m_fbo > 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
		}
	}

	void GLFrameBuffer::unbind()
	{
		if (m_fbo > 0)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}

}