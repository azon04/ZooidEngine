#ifndef __Z_GL_BUFFER_ARRAY__
#define __Z_GL_BUFFER_ARRAY__

#include "Renderer/IGPUBufferArray.h"

#include <GL/glew.h>

namespace ZE
{
	class GLBufferArray : public IGPUBufferArray
	{
	public:

		GLBufferArray() : m_VAO(0) {}

		// IGPUBufferArray implementation
		virtual void SetupBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _computeBuffer) override;

		virtual void Bind() override;
		virtual void Unbind() override;
		virtual void release() override;

		// Bind Layout
		void BindLayout(BufferLayout* layout);

	private:
		GLuint m_VAO;
	};
}
#endif
