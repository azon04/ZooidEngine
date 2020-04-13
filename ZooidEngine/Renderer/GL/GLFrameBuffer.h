#ifndef _Z_GL_FRAMEBUFFER__
#define _Z_GL_FRAMEBUFFER__

#include "Renderer/IGPUFrameBuffer.h"
#include "Utils/Array.h"

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
		virtual void addTextureCubeAttachment(EFrameBuferAttachmentType attachType, IGPUTexture* texture, UInt32 faceIndex, UInt32 attachIndex = 0) override;
		virtual void setupAttachments() override;
		virtual void release() override;
		virtual void bind() override;
		virtual void unbind() override;

		GLuint getFBO() const { return m_fbo; }

	private:
		// GL Frame buffer object
		GLuint m_fbo;
		bool m_bColorAttached;
		Array<GLenum> m_drawBuffers;
	};
}
#endif
