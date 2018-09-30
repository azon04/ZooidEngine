#include "GLTexture.h"
#include "Resources/Texture.h"
#include "Enums.h"

namespace ZE
{ 

	void GLTexture::fromTexture(Texture* texture)
	{
		IGPUTexture::fromTexture(texture);

		GLenum imageFormat = getGLTextureFormat(texture->getTextureFormat());

		glGenTextures(1, &m_textureBuffer);

		glBindTexture(GL_TEXTURE_2D, m_textureBuffer);

		// Wrap functions
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrap(texture->getWrapU()));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrap(texture->getWrapV()));
		// Set filtering and mipmaping function
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getTextureFilter(texture->getMinFilter()));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getTextureFilter(texture->getMagFilter()));

		// #TODO handle multisampling if needed
		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, m_textureRes->getWidth(), m_textureRes->getHeight(), 0, imageFormat, getGLDataType(texture->getDataType()), m_textureRes->getImage());

		glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texture->getBorderColor().m_data);

		if(texture->isGenerateMipMap())
			glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		// #TODO Should we release the memory of the image when the image already loading to GPU?
	}

	void GLTexture::release()
	{
		if (m_textureBuffer != 0)
		{
			glDeleteTextures(1, &m_textureBuffer);
			m_textureBuffer = 0;
		}
	}

	void GLTexture::bind()
	{
		if (m_textureBuffer != 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_textureBuffer);
		}
	}

	void GLTexture::unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}