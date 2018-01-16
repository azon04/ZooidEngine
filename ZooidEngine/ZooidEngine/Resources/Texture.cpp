#include "Texture.h"

#include "ZEngine.h"

#include "External/Image/stb_image.h"

namespace ZE
{
	IMPLEMENT_CLASS_0(Texture)

	Handle Texture::loadTexture(const char* filePath)
	{
		Handle textureHandle(sizeof(Texture));
		int width, height, channel;
		UChar* image = stbi_load(filePath, &width, &height, &channel, STBI_rgb); // #TODO handle image with alpha

		if (image)
		{
			Texture* texture = new(textureHandle) Texture();
			
			texture->m_image = image;
			texture->m_height = height;
			texture->m_width = width;
			texture->m_channel = channel;

		}
		else
		{
			ZEINFO("Unable to open texture file : %s ", filePath);
		}

		return textureHandle;
	}

	void Texture::release()
	{
		// #TODO texture raw data is not part of Memory management since it's loaded using stbi, convert it later.
		if (m_image)
		{
			free(m_image);
			m_image = nullptr;
		}
	}

}

