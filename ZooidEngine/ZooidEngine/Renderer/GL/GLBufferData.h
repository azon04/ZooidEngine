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
			: m_BBO(0)
		{}

		GLBufferData(bool _isStaticBuffer)
			: IGPUBufferData(_isStaticBuffer), m_BBO(0)
		{}

		// IGPUBufferData implementation
		virtual void FromBufferData(BufferData* _bufferData) override;

		virtual void bind() override;
		virtual void unbind() override;
		virtual void release() override;

		FORCEINLINE GLuint getBBO() const { return m_BBO; }

	private:
		GLuint m_BBO;
	};
}

#endif
