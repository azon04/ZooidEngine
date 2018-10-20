#ifndef __ZE_GPU_FRAMEBUFFER_H__
#define __ZE_GPU_FRAMEBUFFER_H__

#include "Utils/PrimitiveTypes.h"
#include "Enums.h"

namespace ZE
{
	class IGPUTexture;
	class IGPURenderBuffer;

	class IGPUFrameBuffer
	{
	public:
		IGPUFrameBuffer() {}

		virtual ~IGPUFrameBuffer() { release(); }

		// create frame buffer
		virtual void create();

		// Attach texture to Framebuffer with attachment type
		virtual void addTextureAttachment(EFrameBuferAttachmentType attachType, IGPUTexture* texture, UInt32 attachIndex = 0) = 0;
		
		// Attach render buffer to frame buffer with attachment type
		virtual void addRenderBufferAttachment(EFrameBuferAttachmentType attachType, IGPURenderBuffer* renderBuffer, UInt32 attachIndex = 0) = 0;

		// Setup attachments
		virtual void setupAttachments() = 0;

		// Release
		virtual void release();

		// Bind Frame Buffer to Render pipeline
		virtual void bind() = 0;

		// Unbind Frame Buffer from Render pipeline
		virtual void unbind() = 0;

	};
}
#endif
