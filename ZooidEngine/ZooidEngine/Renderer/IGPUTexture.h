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

	protected:
		Texture* m_textureRes;
	};
}
#endif
