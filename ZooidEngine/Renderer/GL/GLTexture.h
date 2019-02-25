#ifndef __Z_GL_TEXTURE__
#define __Z_GL_TEXTURE__

#include "Renderer/IGPUTexture.h"
#include "Renderer/BufferLayout.h"

#include <GL/glew.h>

namespace ZE
{
	class GLTexture : public IGPUTexture
	{
	public:
		GLTexture() : m_textureBuffer(0), m_bCube(false) {}

		// IGPUTexture implementation
		virtual void fromTexture(Texture* texture) override;
		virtual void release() override;

		virtual void bind() override;
		virtual void unbind() override;

		GLuint getTextureBuffer() const { return m_textureBuffer; }

	private:
		GLuint m_textureBuffer;
		bool m_bCube;
	};
}
#endif
