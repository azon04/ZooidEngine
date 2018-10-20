#ifndef __ZE_GPU_RENDER_BUFFER_H__
#define __ZE_GPU_RENDER_BUFFER_H__

#include "Utils/PrimitiveTypes.h"
#include "Enums.h"

namespace ZE
{
	class IGPURenderBuffer
	{

	public:
		
		IGPURenderBuffer() {}

		virtual ~IGPURenderBuffer() { release(); }

		// create render buffer
		virtual void create(ERenderBufferFormat format, UInt32 width, UInt32 height);

		// Release GPU Render Buffer
		virtual void release();

		// bind render buffer to render pipeline
		virtual void bind() = 0;

		// unbind render buffer from render pipeline
		virtual void unbind() = 0;
	};
}
#endif
