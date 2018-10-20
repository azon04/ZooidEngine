#ifndef _Z_GL_FRAMEBUFFER__
#define _Z_GL_FRAMEBUFFER__

#include "Renderer/IGPUFrameBuffer.h"

#include <GL/glew.h>

namespace ZE
{
	class IGPUTexture;
	class IGPURenderBuffer;

	class GLFrameBuffer : public IGPUFrameBuffer
	{
	public:
		GLFrameBuffer();

		virtual void create() override;
		virtual void addTextureAttachment(EFrameBuferAttachmentType attachType, IGPUTexture* texture, UInt32 attachIndex = 0) override;
		virtual void addRenderBufferAttachment(EFrameBuferAttachmentType attachType, IGPURenderBuffer* renderBuffer, UInt32 attachIndex = 0) override;
		virtual void setupAttachments() override;
		virtual void release() override;
		virtual void bind() override;
		virtual void unbind() override;

	private:
		// GL Frame buffer object
		GLuint m_fbo;
		bool m_bColorAttached;
		UInt32 m_width;
		UInt32 m_height;
	};
}
#endif
