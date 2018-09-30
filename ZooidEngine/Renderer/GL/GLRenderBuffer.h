#ifndef _Z_GL_RENDERBUFFER__
#define _Z_GL_RENDERBUFFER__

#include "Renderer/IGPURenderBuffer.h"

#include <GL/glew.h>

namespace ZE
{
	class GLRenderBuffer : public IGPURenderBuffer
	{
	public:
		GLRenderBuffer();

		virtual void create(ERenderBufferFormat format, UInt32 width, UInt32 height) override;
		virtual void release() override;
		virtual void bind() override;
		virtual void unbind() override;

		GLuint getRenderBufferObject() const { return m_rbo; }

	private:
		// GL Render Buffer Object
		GLuint m_rbo;

	};
}
#endif
