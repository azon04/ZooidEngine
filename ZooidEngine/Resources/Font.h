#ifndef __ZE_FONT_H__
#define __ZE_FONT_H__

#include "Utils/PrimitiveTypes.h"
#include "GameObjectModel/Object.h"
#include "Memory/Handle.h"
#include "Math/Vector2.h"
#include "Utils/Array.h"

namespace ZE
{
	class GameContext;
	class BufferData;

	struct CharFontDesc
	{
		Vector2 TexCoord;
		Vector2 TexCoordSize;
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

		void release();

		Float32 calculateRenderTextLength(const char* text, Float32 scale);
		Float32 calculateRenderTextLength(String sText, Float32 scale);

		bool generateBufferDataForText(const char* text, Float32 scale, BufferData* bufferData);

	protected:
		Array<CharFontDesc> m_charFontDescs;
		Handle m_hGPUTexture;
	};
}
#endif
