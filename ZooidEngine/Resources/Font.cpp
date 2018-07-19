#include "Font.h"
#include "Memory/MemoryHelper.h"
#include "Texture.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/RenderZooid.h"
#include "ZEGameContext.h"
#include "Renderer/IRenderer.h"
#include "Renderer/BufferData.h"

#include "ft2build.h"
#include FT_FREETYPE_H

namespace ZE
{
	IMPLEMENT_CLASS_0(Font)

	Handle Font::LoadFont(const char* filePath, GameContext* gameContext)
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

		const Int32 fontHeight = 48;
		FT_Set_Pixel_Sizes(face, 0, fontHeight);

		Font* pFont = new(hRes) Font;

		pFont->m_charFontDescs.reset(128);

		// First calculate the texture size
		Vector2 TextureDimension(0.0f, 0.0f);
		
		const Int32 numberPerRow = 15;
		const Int32 padding = 10;
		Vector2 curPos(padding, padding);
		Int32 texSize = 0;
		for (UInt8 c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_BITMAP_METRICS_ONLY))
			{
				ZEERROR("Failed to load Glyph");
				continue;
			}

			pFont->m_charFontDescs[c].TexCoord = curPos;

			curPos.setX(curPos.getX() + padding + face->glyph->bitmap.width);

			if (texSize < curPos.getX() + face->glyph->bitmap.width)
			{
				texSize = curPos.getX() + face->glyph->bitmap.width;
			}

			if ((c+1) % numberPerRow == 0)
			{
				curPos.setX(padding);
				curPos.setY(curPos.getY() + padding + fontHeight);
			}

			if (texSize < curPos.getY() + fontHeight)
			{
				texSize = curPos.getY() + fontHeight;
			}
		}

		// Texture size must be multiply by 4
		texSize = (texSize / 4 + 1) * 4;

		// Init memory for texture atlas
		Handle hTAtlas("Font Atlas", sizeof(char) * texSize * texSize);
		void* pTAtlas = hTAtlas.getObject();

		MemoryHelper::Zero(pTAtlas, hTAtlas.getCapacity());

		// Load Glyph bitmap and copy to the atlas
		curPos.setX(padding);
		curPos.setY(padding);
		for (UInt8 c = 0; c < 128; c++)
		{
			if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			{
				ZEERROR("Failed to load Glyph");
				continue;
			}

			CharFontDesc& charFontDesc = pFont->m_charFontDescs[c];
			charFontDesc.Dimension = Vector2(face->glyph->bitmap.width, face->glyph->bitmap.rows);
			charFontDesc.Advance = face->glyph->advance.x >> 6;
			charFontDesc.Bearing = Vector2(face->glyph->bitmap_left, face->glyph->bitmap_top);
			charFontDesc.TexCoord.setX(charFontDesc.TexCoord.getX() / (Float32)texSize);
			charFontDesc.TexCoord.setY(charFontDesc.TexCoord.getY() / (Float32)texSize);
			charFontDesc.TexCoordSize = charFontDesc.Dimension / (Float32)texSize;

			MemoryHelper::CopyTexture(pTAtlas, texSize, texSize, curPos.getX(), curPos.getY(), 
				face->glyph->bitmap.buffer, face->glyph->bitmap.width, face->glyph->bitmap.rows, sizeof(char));
			
			curPos.setX(curPos.getX() + padding + face->glyph->bitmap.width);

			if ((c+1) % numberPerRow == 0)
			{
				curPos.setX(padding);
				curPos.setY(curPos.getY() + padding + fontHeight);
			}
		}

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
		if (m_hGPUTexture.isValid())
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
			result += scale * m_charFontDescs[c].Advance;
			c = text[index++];
		}

		return result;
	}

	ZE::Float32 Font::calculateRenderTextLength(String sText, Float32 scale)
	{
		return calculateRenderTextLength(sText.const_str(), scale);
	}

	bool Font::generateBufferDataForText(const char* text, Float32 scale, BufferData* bufferData)
	{
		Handle handle = bufferData->getHandle();

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

		for (UInt32 i = 0; i < textLength; i++)
		{
			char c = text[i];
			CharFontDesc& charFontDesc = m_charFontDescs[c];

			Float32 xPos = x + charFontDesc.Bearing.getX() * scale;
			Float32 yPos = y - (charFontDesc.Dimension.getY() - charFontDesc.Bearing.getY()) * scale;

			Float32 w = charFontDesc.Dimension.getX() * scale;
			Float32 h = charFontDesc.Dimension.getY() * scale;
			
			pData[index++] = xPos;
			pData[index++] = yPos + h;
			pData[index++] = charFontDesc.TexCoord.getX();
			pData[index++] = charFontDesc.TexCoord.getY();

			pData[index++] = xPos;
			pData[index++] = yPos;
			pData[index++] = charFontDesc.TexCoord.getX();
			pData[index++] = charFontDesc.TexCoord.getY() + charFontDesc.TexCoordSize.getY();

			pData[index++] = xPos + w;
			pData[index++] = yPos;
			pData[index++] = charFontDesc.TexCoord.getX() + charFontDesc.TexCoordSize.getX();
			pData[index++] = charFontDesc.TexCoord.getY() + charFontDesc.TexCoordSize.getY();

			pData[index++] = xPos;
			pData[index++] = yPos + h;
			pData[index++] = charFontDesc.TexCoord.getX();
			pData[index++] = charFontDesc.TexCoord.getY();

			pData[index++] = xPos + w;
			pData[index++] = yPos;
			pData[index++] = charFontDesc.TexCoord.getX() + charFontDesc.TexCoordSize.getX();
			pData[index++] = charFontDesc.TexCoord.getY() + charFontDesc.TexCoordSize.getY();

			pData[index++] = xPos + w;
			pData[index++] = yPos + h;
			pData[index++] = charFontDesc.TexCoord.getX() + charFontDesc.TexCoordSize.getX();
			pData[index++] = charFontDesc.TexCoord.getY();

			x += charFontDesc.Advance * scale;
		}

		bufferData->setData(handle, 4 * sizeof(Float32), 6 * textLength);

		return true;
	}

}