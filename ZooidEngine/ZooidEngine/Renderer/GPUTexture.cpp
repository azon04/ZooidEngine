#include "GPUTexture.h"

namespace ZE
{
	void GPUTexture::FromTexture(Texture* texture)
	{
		m_textureRes = texture;
		
#if Z_RENDER_OPENGL
		glGenTextures(1, &m_textureBuff);

		glBindTexture(GL_TEXTURE_2D, m_textureBuff);

		// #TODO make these parameterize
		// Wrap functions
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
		// Set filtering and mipmaping function
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// #TODO handle multisampling if needed
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_textureRes->getWidth(), m_textureRes->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_textureRes->getImage());

		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);
		// #TODO Should we release the memory of the image when the image already loading to GPU?
#endif

	}

	void GPUTexture::release()
	{
#if Z_RENDER_OPENGL
		if (m_textureBuff != 0)
		{
			glDeleteTextures(1, &m_textureBuff);
			m_textureBuff = 0;
		}
#endif
	}

	void GPUTexture::Bind()
	{
#if Z_RENDER_OPENGL
		if (m_textureBuff != 0)
		{
			glBindTexture(GL_TEXTURE_2D, m_textureBuff);
		}
#endif
	}

	void GPUTexture::Unbind()
	{
#if Z_RENDER_OPENGL
		glBindTexture(GL_TEXTURE_2D, 0);
#endif
	}

}