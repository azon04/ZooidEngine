#include "GLTexture.h"
#include "Resources/Texture.h"

namespace ZE
{ 

	void GLTexture::fromTexture(Texture* texture)
	{
		IGPUTexture::fromTexture(texture);

		GLenum imageFormat;
		switch (texture->getChannel())
		{
		case 1:
			imageFormat = GL_RED;
			break;
		case 3:
			imageFormat = GL_RGB;
			break;
		case 4:
			imageFormat = GL_RGBA;
			break;
		default:
			imageFormat = GL_RGB;
			break;
		}

		glGenTextures(1, &m_textureBuffer);

		glBindTexture(GL_TEXTURE_2D, m_textureBuffer);

		// Wrap functions
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getTextureWrap(texture->getWrapU()));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getTextureWrap(texture->getWrapV()));
		// Set filtering and mipmaping function
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getTextureFilter(texture->getMinFilter()));
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getTextureFilter(texture->getMagFilter()));

		// #TODO handle multisampling if needed
		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, m_textureRes->getWidth(), m_textureRes->getHeight(), 0, imageFormat, GL_UNSIGNED_BYTE, m_textureRes->getImage());

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

	GLint GLTexture::getTextureWrap(TextureWrap wrap)
	{
		switch (wrap)
		{
		case REPEAT:
			return GL_REPEAT;
		case MIRRORED_REPEAT:
			return GL_MIRRORED_REPEAT;
		case CLAMP_TO_EDGE:
			return GL_CLAMP_TO_EDGE;
		default:
			return 0;
		}
	}

	GLint GLTexture::getTextureFilter(TextureFilter filter)
	{
		switch (filter)
		{
		case NEAREST:
			return GL_NEAREST;
		case NEAREST_MIPMAP_NEAREST:
			return GL_NEAREST_MIPMAP_NEAREST;
		case LINEAR_MIPMAP_NEAREST:
			return GL_LINEAR_MIPMAP_NEAREST;
		case NEAREST_MIPMAP_LINEAR:
			return GL_NEAREST_MIPMAP_LINEAR;
		case LINEAR:
			return GL_LINEAR;
		case LINEAR_MIPMAP_LINEAR:
			return GL_LINEAR_MIPMAP_LINEAR;
		default:
			return 0;
		}
	}

}