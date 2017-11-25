#ifndef __ZE_GPU_TEXTURE_H__
#define	__ZE_GPU_TEXTURE_H__

#include "Resources/Texture.h"

#if Z_RENDER_OPENGL
#include <GL/glew.h>
#endif

namespace ZE
{
	class GPUTexture
	{
	public:
		GPUTexture()
		{
#if Z_RENDER_OPENGL
			m_textureBuff = 0;
#endif
		}

		virtual ~GPUTexture() { release(); }

		void FromTexture(Texture* texture);
		void release();

		void Bind();
		void Unbind();

	protected:

#if Z_RENDER_OPENGL
		GLuint m_textureBuff;
#endif

		Texture* m_textureRes;
	};
}
#endif
