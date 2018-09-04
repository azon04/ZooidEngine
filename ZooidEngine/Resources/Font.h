#ifndef __ZE_FONT_H__
#define __ZE_FONT_H__

#include "Utils/PrimitiveTypes.h"
#include "GameObjectModel/Object.h"
#include "Memory/Handle.h"
#include "Math/Vector2.h"
#include "Utils/Array.h"
#include "Utils/HashMap.h"

namespace ZE
{
	class GameContext;
	class BufferData;
	class IGPUTexture;

	enum FontRenderMethod : UInt8
	{
		FONT_RENDER_NONE = 0,
		FONT_RENDER_TEX = 1,
		FONT_RENDER_SDF = 2,
		FONT_RENDER_PSDF = 3,
		FONT_RENDER_MSDF = 4
	};

	struct CharFontDesc
	{
		Vector2 TexCoord;
		Vector2 Bearing;
		Vector2 Dimension;
		UInt32 Advance;
	};

	class Font : public Object
	{
		DEFINE_CLASS(Font)

	public:
		Font() {}

		// Load font from file
		static Handle LoadFont(const char* filePath, GameContext* _gameContext);
		static Handle LoadFontFile(const char* filePath, GameContext* _gameContext);

		void release();

		Float32 calculateRenderTextLength(const char* text, Float32 scale);
		Float32 calculateRenderTextLength(String sText, Float32 scale);

		Float32 getFontHeight() const { return m_fontHeight; }
		FontRenderMethod getFontRenderMethod() const { return m_renderMethod; }
		bool generateBufferDataForText(const char* text, Float32 scale, BufferData* bufferData, bool bNormalize = false);

		IGPUTexture* getGPUTexture();

	private:

		Int32 getCharIndex(char c);
		static FontRenderMethod getRenderMethodFromString(const char* renderMethodString);

	protected:
		Array<CharFontDesc> m_charFontDescs;
		HashMap<UInt32, UInt32> m_charMap;
		Handle m_hGPUTexture;
		Float32 m_fontHeight;
		Int32 m_textureSize;
		FontRenderMethod m_renderMethod = FONT_RENDER_NONE;
		bool m_bNeedToClearTex = true;
	};
}
#endif
