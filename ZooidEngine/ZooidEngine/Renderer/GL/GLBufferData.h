#ifndef __Z_GL_BUFFER_DATA__
#define __Z_GL_BUFFER_DATA__

#include "Renderer/IGPUBufferData.h"

#include <GL/glew.h>

namespace ZE
{
	class GLBufferData : public IGPUBufferData
	{
	public:

		GLBufferData()
		{
			m_BBO = 0;
		}

		GLBufferData(bool _isStaticBuffer)
			: IGPUBufferData(_isStaticBuffer)
		{
			m_BBO = 0;
		}

		virtual void FromBufferData(BufferData* _bufferData) override;

		virtual void Bind() override;
		virtual void UnBind() override;
		virtual void release() override;

		GLuint m_BBO;

	};
}

#endif
