#ifndef __Z_GL_BUFFER_ARRAY__
#define __Z_GL_BUFFER_ARRAY__

#include "Renderer/IGPUBufferArray.h"

#include <GL/glew.h>

namespace ZE
{
	class BufferLayout;

	class GLBufferArray : public IGPUBufferArray
	{
	public:

		GLBufferArray() : m_VAO(0) {}

		// IGPUBufferArray implementation
		virtual void SetupBufferArray(IGPUBufferData* _vertexBuffer, IGPUBufferData* _indexBuffer, IGPUBufferData* _computeBuffer) override;
		virtual void SetupBufferArray(IGPUBufferData** _vertexBuffers, UInt32 _vertexBufferCount, IGPUBufferData** _indexBuffers, UInt32 _indexBufferCount, IGPUBufferData** _computeBuffers = nullptr, UInt32 _computeBufferCount = 0);
		virtual void AddVertexBuffer(IGPUBufferData* _vertexBuffer);
		virtual void AddIndexBuffer(IGPUBufferData* _indexBuffer);
		virtual void AddComputeBuffer(IGPUBufferData* _computeBuffer);

		virtual void bind() override;
		virtual void unbind() override;
		virtual void release() override;

		// Bind Layout
		void BindLayout(BufferLayout* layout, UInt32 offset = 0);

	private:
		GLuint m_VAO;
		UInt32 m_vertexLayoutSize;
	};
}
#endif
