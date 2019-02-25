#include "Texture.h"

#include "External/Image/stb_image.h"
#include "Utils/Array.h"
#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"

namespace ZE
{
	IMPLEMENT_CLASS_0(Texture)

	Handle Texture::loadTexture(const char* filePath)
	{
		Handle textureHandle(sizeof(Texture));
		if (StringFunc::FindLast(filePath, ".cube") >= 0)
		{
			FileReader reader(filePath);

			// Read Header
			char buffer[255];
			reader.readNextString(buffer);
			
			// Read number of faces
			int faces = reader.readNextInt();

			ZASSERT(faces == 6, "Not cubemap!");

			Texture* texture = new(textureHandle) Texture();

			int width, height, channel;

			for (int i = 0; i < faces; i++)
			{
				// Read index
				reader.readNextString(buffer);

				// Read file path
				reader.readNextString(buffer);

				UChar* image = stbi_load(GetResourcePath(buffer).c_str(), &width, &height, &channel, 0);

				if (i == 0)
				{
					texture->loadFromBuffer(image, width, height, 1, channel, faces);
				}
				else
				{
					ZASSERT(texture->m_width == width, "Width for cubemap for all textures should be the same");
					ZASSERT(texture->m_height == height, "Height for cubemap for all textures should be the same");
					ZASSERT(texture->m_channel == channel, "Channel for cubemap for all textures should be the same");

					texture->addBufferToTexture(image, i);
				}
			}

			reader.close();
		}
		else
		{
			int width, height, channel;
			UChar* image = stbi_load(filePath, &width, &height, &channel, 0);

			if (image)
			{
				Texture* texture = new(textureHandle) Texture();

				texture->loadFromBuffer(image, width, height, 1, channel);
			}
			else
			{
				ZEINFO("Unable to open texture file : %s ", filePath);
			}
		}

		return textureHandle;
	}

	void Texture::loadFromBuffer(void* firstBuffer, UInt32 _width, UInt32 _height, UInt32 _depth, UInt32 _channel, UInt32 _faces)
	{
		m_images.reset(_faces);
		m_images.clear();
		m_images.push_back((UChar*)firstBuffer);

		for (int i = 1; i < _faces; i++)
		{
			m_images.push_back(nullptr);
		}

		m_width = _width;
		m_height = _height;
		m_depth = _depth;
		m_channel = _channel;
		m_face = _faces;

		switch (m_channel)
		{
		case 1:
			m_format = TEX_RED;
			break;
		case 2:
			m_format = TEX_RED_GREEN;
			break;
		case 3:
			m_format = TEX_RGB;
			break;
		case 4:
			m_format = TEX_RGBA;
			break;
		}
	}

	void Texture::addBufferToTexture(void* buffer, UInt32 faceIndex)
	{
		ZASSERT(faceIndex < m_face, "Face Index is invalid");
		ZASSERT(buffer);

		m_images[faceIndex] = (UChar*)buffer;
	}

	void Texture::createEmpty(UInt32 _width, UInt32 _height, UInt32 _depth, ETextureFormat _format, UInt32 _faces)
	{
		m_width = _width;
		m_height = _height;
		m_format = _format;
		m_depth = _depth;
		m_face = _faces;
		m_images.reset(m_face);
		for (UInt32 i = 0; i < m_face; i++)
		{
			m_images.push_back(nullptr);
		}
	}

	void Texture::release()
	{
		// #TODO texture raw data is not part of Memory management since it's loaded using stbi, convert it later.
		if (m_images.size())
		{
			for (UInt32 i = 0; i < m_face; i++)
			{
				free(m_images[i]);
				m_images[i] = nullptr;
			}
		}
	}

}

