#ifndef __ZE_TEXT_MESH_H__
#define __ZE_TEXT_MESH_H__

#include "GameObjectModel/Object.h"

namespace ZE
{
	class IGPUBufferArray;
	class IGPUBufferData;
	class Font;

	class TextMesh : public Object
	{
		DEFINE_CLASS(TextMesh)

	public:

		TextMesh()
		: m_bufferArray(NULL) 
		{}

		FORCEINLINE IGPUBufferArray* getGPUBufferArray() const { return m_bufferArray; }

		void init(Font* _font, const char* text);
		void generateTextMesh(const char* text, bool bNormalizeHeight = false);
		void setFont(Font* _font);

	protected:
		IGPUBufferArray* m_bufferArray;
		Font* m_font;
	};
}
#endif
