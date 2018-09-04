#ifndef __ZE_GPU_TEXTURE_H__
#define	__ZE_GPU_TEXTURE_H__

#include "Resources/Texture.h"

namespace ZE
{
	class IGPUTexture
	{
	public:
		IGPUTexture()
		{
		}

		virtual ~IGPUTexture() { release(); }

		// From CPU Texture
		virtual void fromTexture(Texture* texture);

		// Release GPU Texture
		virtual void release();

		// bind texture to rendering pipeline
		virtual void bind() = 0;

		// unbind texture from rendering pipeline
		virtual void unbind() = 0;

		// Get height of the texture in pixel
		UInt32 getHeight() { return m_height; }

		// Get Wwidth of the texture in pixel
		UInt32 getWidth() { return m_width; }

	protected:
		Texture* m_textureRes;
		UInt32 m_width;
		UInt32 m_height;
	};
}
#endif
