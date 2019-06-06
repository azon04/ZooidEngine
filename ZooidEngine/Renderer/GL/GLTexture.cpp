#include "GLTexture.h"
#include "Resources/Texture.h"
#include "Enums.h"
#include "Renderer/Structs.h"

namespace ZE
{ 

	void GLTexture::create(const TextureCreateDesc& createDesc)
	{
		TextureFormatStruct textureFormat = TextureFormats[createDesc.TextureFormat];
		GLenum imageFormat = getGLTextureFormat(textureFormat.TextureFormat);
		GLenum baseFormat = getGLTextureFormat(textureFormat.BaseFormat);

		GLenum imageType = GL_TEXTURE_2D;

		m_bCube = createDesc.FaceCount == 6;
		if (m_bCube)
		{
			imageType = GL_TEXTURE_CUBE_MAP;
		}

		glGenTextures(1, &m_textureBuffer);

		glBindTexture(imageType, m_textureBuffer);

		// Wrap functions
		glTexParameteri(imageType, GL_TEXTURE_WRAP_S, getTextureWrap(createDesc.WrapU));
		glTexParameteri(imageType, GL_TEXTURE_WRAP_T, getTextureWrap(createDesc.WrapV));
		// Set filtering and mipmaping function
		glTexParameteri(imageType, GL_TEXTURE_MIN_FILTER, getTextureFilter(createDesc.MinFilter));
		glTexParameteri(imageType, GL_TEXTURE_MAG_FILTER, getTextureFilter(createDesc.MagFilter));

		if (m_bCube)
		{
			for (int i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imageFormat, createDesc.Width, createDesc.Height, 0, baseFormat, getGLDataType(createDesc.DataType), NULL);
			}
		}
		else
		{
			// #TODO handle multisampling if needed
			glTexImage2D(imageType, 0, imageFormat, createDesc.Width, createDesc.Height, 0, baseFormat, getGLDataType(createDesc.DataType), NULL);
		}

		glTexParameterfv(imageType, GL_TEXTURE_BORDER_COLOR, createDesc.BorderColor.m_data);

		if (createDesc.bGenerateMipMap)
			glGenerateMipmap(imageType);

		glBindTexture(imageType, 0);
		// #TODO Should we release the memory of the image when the image already loading to GPU?
	}

	void GLTexture::fromTexture(Texture* texture)
	{
		IGPUTexture::fromTexture(texture);

		TextureFormatStruct textureFormat = TextureFormats[texture->getTextureFormat()];
		GLenum imageFormat = getGLTextureFormat(textureFormat.TextureFormat);
		GLenum baseFormat = getGLTextureFormat(textureFormat.BaseFormat);
		
		GLenum imageType = GL_TEXTURE_2D;
		
		m_bCube = texture->getNumberOfFaces() == 6;
		if (m_bCube)
		{
			imageType = GL_TEXTURE_CUBE_MAP;
		}

		glGenTextures(1, &m_textureBuffer);

		glBindTexture(imageType, m_textureBuffer);

		// Wrap functions
		glTexParameteri(imageType, GL_TEXTURE_WRAP_S, getTextureWrap(texture->getWrapU()));
		glTexParameteri(imageType, GL_TEXTURE_WRAP_T, getTextureWrap(texture->getWrapV()));
		// Set filtering and mipmaping function
		glTexParameteri(imageType, GL_TEXTURE_MIN_FILTER, getTextureFilter(texture->getMinFilter()));
		glTexParameteri(imageType, GL_TEXTURE_MAG_FILTER, getTextureFilter(texture->getMagFilter()));

		if (m_bCube)
		{
			for (int i = 0; i < 6; i++)
			{
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, imageFormat, m_textureRes->getWidth(), m_textureRes->getHeight(), 0, baseFormat, getGLDataType(texture->getDataType()), m_textureRes->getImage(i));
			}
		}
		else
		{
			// #TODO handle multisampling if needed
			glTexImage2D(imageType, 0, imageFormat, m_textureRes->getWidth(), m_textureRes->getHeight(), 0, baseFormat, getGLDataType(texture->getDataType()), m_textureRes->getImage());
		}
		
		glTexParameterfv(imageType, GL_TEXTURE_BORDER_COLOR, texture->getBorderColor().m_data);

		if(texture->isGenerateMipMap())
			glGenerateMipmap(imageType);

		glBindTexture(imageType, 0);
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
			GLenum imageType = GL_TEXTURE_2D;

			if (m_bCube)
			{
				imageType = GL_TEXTURE_CUBE_MAP;
			}

			glBindTexture(imageType, m_textureBuffer);
		}
	}

	void GLTexture::unbind()
	{
		GLenum imageType = GL_TEXTURE_2D;

		if (m_bCube)
		{
			imageType = GL_TEXTURE_CUBE_MAP;
		}

		glBindTexture(imageType, 0);
	}
}