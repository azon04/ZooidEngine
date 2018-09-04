#ifndef __Z_GL_TEXTURE__
#define __Z_GL_TEXTURE__

#include "Renderer/IGPUTexture.h"

#include <GL/glew.h>

namespace ZE
{
	class GLTexture : public IGPUTexture
	{
	public:
		GLTexture() : m_textureBuffer(0) {}

		// IGPUTexture implementation
		virtual void fromTexture(Texture* texture) override;
		virtual void release() override;

		virtual void bind() override;
		virtual void unbind() override;

	private:
		GLuint m_textureBuffer;

		GLint getTextureWrap(TextureWrap wrap);
		GLint getTextureFilter(TextureFilter filter);
	};
}
#endif
