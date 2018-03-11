#include "GLTexture.h"
#include "Resources/Texture.h"

namespace ZE
{ 

	void GLTexture::FromTexture(Texture* texture)
	{
		IGPUTexture::FromTexture(texture);

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

		// #TODO make these parameterize
		// Wrap functions
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		// Set filtering and mipmaping function
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// #TODO handle multisampling if needed
		glTexImage2D(GL_TEXTURE_2D, 0, imageFormat, m_textureRes->getWidth(), m_textureRes->getHeight(), 0, imageFormat, GL_UNSIGNED_BYTE, m_textureRes->getImage());

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

	void GLTexture::Bind()
	{
		if (m_textureBuffer != 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_textureBuffer);
		}
	}

	void GLTexture::Unbind()
	{
		glBindTexture(GL_TEXTURE_2D, 0);
	}

}