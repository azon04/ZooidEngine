#include "Font.h"

#include "Texture.h"
#include "ZEGameContext.h"

#include "Memory/MemoryHelper.h"

#include "FileSystem/DirectoryHelper.h"
#include "FileSystem/FileReader.h"

#include "Renderer/IGPUTexture.h"
#include "Renderer/RenderZooid.h"
#include "Renderer/IRenderer.h"
#include "Renderer/BufferData.h"
#include "Renderer/BufferLayout.h"

#include "Utils/StringFunc.h"

#include "ResourceManagers/TextureManager.h"

// Outer engine includes
#include "ft2build.h"
#include FT_FREETYPE_H

namespace ZE
{
	IMPLEMENT_CLASS_0(Font)

	ZE::Handle Font::LoadFont(const char* filePath, GameContext* _gameContext)
	{
		Handle hRes("Font", sizeof(Font));

		FileReader reader(filePath);

		char buffer[255];

		reader.readNextString(buffer);

		if (StringFunc::Compare(buffer, "FontFile") == 0)
		{
			// Using Font File
			hRes = LoadFontFile(filePath, _gameContext);
			reader.close();
			return hRes;
		}

		
		Font* pFont = new(hRes) Font();

		// Read "Image"
		reader.readNextString(buffer);

		String texturePath = GetLocationPath(filePath);
		texturePath += buffer;

		pFont->m_hGPUTexture = TextureManager::GetInstance()->loadResource(texturePath.const_str());
		pFont->m_textureSize = pFont->getGPUTexture()->getWidth();
		pFont->m_bNeedToClearTex = false;

		// Read "Method"
		reader.readNextString(buffer);
		reader.readNextString(buffer);

		pFont->m_renderMethod = getRenderMethodFromString(buffer);

		// Read "GlyphHeight"
		reader.readNextString(buffer);

		pFont->m_fontHeight = reader.readNextFloat();

		// Read Glyph Count
		reader.readNextString(buffer);
		int count = reader.readNextInt();

		pFont->m_charFontDescs.reset(count);
		pFont->m_charMap.reset(count);
		for (int i = 0; i < count; i++)
		{
			int charCode = reader.readNextInt();
			CharFontDesc& desc = pFont->m_charFontDescs[i];
			desc.TexCoord.m_x = reader.readNextInt() / static_cast<float> (pFont->m_textureSize);
			desc.TexCoord.m_y = reader.readNextInt() / static_cast<float> (pFont->m_textureSize);
			desc.Dimension.m_x = reader.readNextFloat();
			desc.Dimension.m_y = reader.readNextFloat();
			desc.Bearing.m_x = reader.readNextFloat();
			desc.Bearing.m_y = reader.readNextFloat();
			desc.Advance = reader.readNextInt();

			pFont->m_charMap.put(charCode, i);
		}

		return hRes;
	}

	Handle Font::LoadFontFile(const char* filePath, GameContext* gameContext)
	{
		Handle hRes("Font", sizeof(Font));

		FT_Library ft;
		if (FT_Init_FreeType(&ft))
		{
			ZEERROR("Couldn't init FreeType Library");
			return hRes;
		}

		FT_Face face;
		if (FT_New_Face(ft, filePath, 0, &face))
		{
			ZEERROR("Couldn't load font");
			FT_Done_FreeType(ft);
			return hRes;
		}

		const Int32 fontHeight = 32;
		FT_Set_Pixel_Sizes(face, 0, fontHeight);

		Font* pFont = new(hRes) Font;

		// Font attributes
		UInt32 charCount = face->num_glyphs;
		
		pFont->m_charFontDescs.reset(charCount);
		pFont->m_fontHeight = fontHeight;

		// First calculate the texture size
		Vector2 TextureDimension(0.0f, 0.0f);
		const Float32 padding = 10.0f;
		
		// Calculate possible number per row
		Int32 numberPerRow = (Int32)(sqrt((fontHeight + padding) / (0.5f * fontHeight + padding) * charCount));

		Vector2 curPos(padding, padding);
		Int32 texSize = 0;
		for (UInt32 c = 0; c < charCount; c++)
		{
			if (FT_Load_Glyph(face, c, FT_LOAD_BITMAP_METRICS_ONLY))
			{
				ZEERROR("Failed to load Glyph");
				continue;
			}

			pFont->m_charFontDescs[c].TexCoord = curPos;

			curPos.setX(curPos.getX() + padding + face->glyph->bitmap.width);

			if (texSize < static_cast<Int32>(curPos.getX() + face->glyph->bitmap.width))
			{
				texSize = static_cast<Int32>(curPos.getX()) + face->glyph->bitmap.width;
			}

			if ((c+1) % numberPerRow == 0)
			{
				curPos.setX(padding);
				curPos.setY(curPos.getY() + padding + pFont->m_fontHeight);
			}

			if (texSize < static_cast<Int32>(curPos.getY() + pFont->m_fontHeight))
			{
				texSize = static_cast<Int32>(curPos.getY() + pFont->m_fontHeight);
			}
		}

		// Texture size must be multiply by 4
		texSize = (texSize / 4 + 1) * 4;
		pFont->m_textureSize = texSize;

		// Init memory for texture atlas
		Handle hTAtlas("Font Atlas", sizeof(char) * texSize * texSize);
		void* pTAtlas = hTAtlas.getObject();

		MemoryHelper::Zero(pTAtlas, hTAtlas.getCapacity());

		// Load Glyph bitmap and copy to the atlas
		curPos.setX(padding);
		curPos.setY(padding);
		for (UInt32 c = 0; c < charCount; c++)
		{
			if (FT_Load_Glyph(face, c, FT_LOAD_RENDER))
			{
				ZEERROR("Failed to load Glyph");
				continue;
			}

			CharFontDesc& charFontDesc = pFont->m_charFontDescs[c];
			charFontDesc.Dimension = Vector2(face->glyph->bitmap.width * 1.0f, face->glyph->bitmap.rows * 1.0f);
			charFontDesc.Advance = face->glyph->advance.x >> 6;
			charFontDesc.Bearing = Vector2(face->glyph->bitmap_left * 1.0f, face->glyph->bitmap_top * 1.0f);
			charFontDesc.TexCoord.setX(charFontDesc.TexCoord.getX() / (Float32)texSize);
			charFontDesc.TexCoord.setY(charFontDesc.TexCoord.getY() / (Float32)texSize);

			MemoryHelper::CopyTexture(pTAtlas, texSize, texSize, static_cast<Int32>(curPos.getX()), static_cast<Int32>(curPos.getY()), 
				face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, sizeof(char));
			
			curPos.setX(curPos.getX() + padding + face->glyph->bitmap.width);

			if ((c+1) % numberPerRow == 0)
			{
				curPos.setX(padding);
				curPos.setY(curPos.getY() + padding + fontHeight);
			}
		}

		// Generate HashMap
		UInt32 glyphIndex = 0;
		UInt32 charCode = FT_Get_First_Char(face, &glyphIndex);
		pFont->m_charMap.reset(charCount);
		while (glyphIndex != 0)
		{
			pFont->m_charMap.put(charCode, glyphIndex);
			charCode = FT_Get_Next_Char(face, charCode, &glyphIndex);
		}

		// #TODO this should be handled by TextureManager ???
		Handle hTexture("Font Texture", sizeof(Texture));
		Texture* pTexture = new(hTexture) Texture;
		pTexture->loadFromBuffer(pTAtlas, texSize, texSize, 1);

		gameContext->getRenderer()->AcquireRenderThreadOwnership();
		
		pFont->m_hGPUTexture = gameContext->getRenderZooid()->CreateRenderTexture();
		IGPUTexture* gpuTexture = pFont->m_hGPUTexture.getObject<IGPUTexture>();
		gpuTexture->fromTexture(pTexture);

		gameContext->getRenderer()->ReleaseRenderThreadOwnership();

		FT_Done_Face(face);
		FT_Done_FreeType(ft);

		hTAtlas.release();

		return hRes;
		
	}

	void Font::release()
	{
		if (m_hGPUTexture.isValid() && m_bNeedToClearTex)
		{
			IGPUTexture* pTexture = m_hGPUTexture.getObject<IGPUTexture>();
			pTexture->release();
			m_hGPUTexture.release();
		}
	}

	ZE::Float32 Font::calculateRenderTextLength(const char* text, Float32 scale)
	{
		int index = 0;
		char c = text[index++];
		Float32 result = 0.0f;
		while (c != '\0')
		{
			result += scale * m_charFontDescs[getCharIndex(c)].Advance;
			c = text[index++];
		}

		return result;
	}

	ZE::Float32 Font::calculateRenderTextLength(String sText, Float32 scale)
	{
		return calculateRenderTextLength(sText.const_str(), scale);
	}

	bool Font::generateBufferDataForText(const char* text, Float32 scale, BufferData* bufferData, bool bNormalize)
	{
		Handle handle = bufferData->getHandle();
		bufferData->setBufferLayout(BUFFER_LAYOUT_V4);

		Int32 textLength = StringFunc::Length(text);
		size_t totalSize = textLength * (6 * (4 * sizeof(Float32)));
		if (handle.isValid())
		{
			if (handle.getCapacity() < totalSize)
			{
				handle.release();
				handle = Handle("TextBuffer", totalSize);
			}
		}
		else
		{
			handle = Handle("TextBuffer", totalSize);
		}

		float* pData = handle.getObject<float>();

		UInt32 index = 0;
		Float32 x = 0.0f;
		Float32 y = 0.0f;
		Float32 normalizeFactor = bNormalize ? 1.0f / Float32(m_fontHeight) : 1.0f;

		for (Int32 i = 0; i < textLength; i++)
		{
			char c = text[i];
			CharFontDesc& charFontDesc = m_charFontDescs[getCharIndex(c)];

			Float32 xPos = x + charFontDesc.Bearing.getX() * scale * normalizeFactor ;
			Float32 yPos = y - (charFontDesc.Dimension.getY() - charFontDesc.Bearing.getY()) * scale * normalizeFactor;

			Float32 w = charFontDesc.Dimension.getX() * scale * normalizeFactor;
			Float32 h = charFontDesc.Dimension.getY() * scale * normalizeFactor;
			
			Vector2 texCoordSize = charFontDesc.Dimension / static_cast<Float32>(m_textureSize);

			pData[index++] = xPos;
			pData[index++] = yPos + h;
			pData[index++] = charFontDesc.TexCoord.getX();
			pData[index++] = charFontDesc.TexCoord.getY();

			pData[index++] = xPos;
			pData[index++] = yPos;
			pData[index++] = charFontDesc.TexCoord.getX();
			pData[index++] = charFontDesc.TexCoord.getY() + texCoordSize.getY();

			pData[index++] = xPos + w;
			pData[index++] = yPos;
			pData[index++] = charFontDesc.TexCoord.getX() + texCoordSize.getX();
			pData[index++] = charFontDesc.TexCoord.getY() + texCoordSize.getY();

			pData[index++] = xPos;
			pData[index++] = yPos + h;
			pData[index++] = charFontDesc.TexCoord.getX();
			pData[index++] = charFontDesc.TexCoord.getY();

			pData[index++] = xPos + w;
			pData[index++] = yPos;
			pData[index++] = charFontDesc.TexCoord.getX() + texCoordSize.getX();
			pData[index++] = charFontDesc.TexCoord.getY() + texCoordSize.getY();

			pData[index++] = xPos + w;
			pData[index++] = yPos + h;
			pData[index++] = charFontDesc.TexCoord.getX() + texCoordSize.getX();
			pData[index++] = charFontDesc.TexCoord.getY();

			x += charFontDesc.Advance * scale * normalizeFactor;
		}

		bufferData->setData(handle, 4 * sizeof(Float32), 6 * textLength);

		return true;
	}

	ZE::IGPUTexture* Font::getGPUTexture()
	{
		if (m_hGPUTexture.isValid())
		{
			return m_hGPUTexture.getObject<IGPUTexture>();
		}
		return nullptr;
	}

	ZE::Int32 Font::getCharIndex(char c)
	{
		int index;
		if (m_charMap.getIndex((UInt32)c, index))
		{
			return m_charMap.getValue(index);
		}

		return 0;
	}

	ZE::FontRenderMethod Font::getRenderMethodFromString(const char* renderMethodString)
	{
		if (StringFunc::Compare(renderMethodString, "TEXTURE") == 0)
		{
			return FONT_RENDER_TEX;
		}
		else if (StringFunc::Compare(renderMethodString, "SDF") == 0)
		{
			return FONT_RENDER_SDF;
		}
		else if (StringFunc::Compare(renderMethodString, "PSDF") == 0)
		{
			return FONT_RENDER_PSDF;
		}
		else if (StringFunc::Compare(renderMethodString, "MSDF") == 0)
		{
			return FONT_RENDER_MSDF;
		}
		return FONT_RENDER_NONE;
	}

}