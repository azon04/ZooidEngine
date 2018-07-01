#ifndef __ZE_TEXTURE_H__
#define __ZE_TEXTURE_H__

#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"
#include "GameObjectModel/Object.h"

namespace ZE
{
	class Texture : public Object
	{
		DEFINE_CLASS(Texture)

	public:
		Texture() {}

		// load texture from file
		static Handle loadTexture(const char* filePath);

		// Get pointer to image
		FORCEINLINE UChar* getImage() const { return m_image; }

		FORCEINLINE UInt32 getWidth() const { return m_width; }
		FORCEINLINE UInt32 getHeight() const { return m_height; }
		
		FORCEINLINE UInt32 getChannel() const { return m_channel; }

		void release();


	protected:
		UChar* m_image;
		UInt32 m_width;
		UInt32 m_height;
		UInt32 m_channel;
	};
};
#endif
