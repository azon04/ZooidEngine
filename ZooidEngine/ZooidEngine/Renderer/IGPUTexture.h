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

		virtual void FromTexture(Texture* texture);
		virtual void release();

		virtual void Bind() = 0;
		virtual void Unbind() = 0;

	protected:

		Texture* m_textureRes;
	};
}
#endif
