#ifndef __ZE_TEXTURE_H__
#define __ZE_TEXTURE_H__

#include "Utils/PrimitiveTypes.h"
#include "MemoryManagement/Handle.h"

namespace ZE
{
	class Texture
	{
	
	public:
		Texture() {}

		static Handle loadTexture(const char* filePath);

		UChar* getImage() const { return m_image; }
		UInt32 getWidth() const { return m_width; }
		UInt32 getHeight() const { return m_height; }

		void release();

	protected:
		UChar* m_image;
		UInt32 m_width;
		UInt32 m_height;
		UInt32 m_channel;
	};
};
#endif
