#ifndef __Z_GPU_BUFFER__
#define __Z_GPU_BUFFER__

#include "Utils/PrimitiveTypes.h"
#include "Utils/Macros.h"

#include "Enums.h"

#define MAX_LAYOUT 16

namespace ZE 
{

	class BufferLayout;
	class BufferData;

	class IGPUBufferData 
	{
		friend class BufferManager;

	public:
		IGPUBufferData() 
		{
			m_isStatic = true;
		}

		IGPUBufferData(bool _isStaticBuffer)
			: IGPUBufferData()
		{
			m_isStatic = _isStaticBuffer;
		}

		virtual ~IGPUBufferData();

		// Load GPU Buffer Data from Buffer Data
		virtual void FromBufferData(BufferData* _bufferData);

		// Setup buffer layout
		void SetupLayout(BufferLayout* _layouts);

		// Bind GPU Buffer into render pipeline
		virtual void bind() = 0;

		// Unbind GPU buffer from render pipeline
		virtual void unbind() = 0;

		// Release GPU buffer data
		virtual void release() {};

		// Refresh data to GPU
		virtual void refresh() = 0;

		// Bind and refresh
		virtual void bindAndRefresh() = 0;

		// Set if the buffer is static
		void setStatic(bool _bStatic) { m_isStatic = _bStatic; }

		bool isStatic() const { return m_isStatic; }
	
		// get data count
		FORCEINLINE Int32 getDataCount() const { return m_dataCount; }

		// Get GPU Binding Index
		FORCEINLINE Int32 getBindingIndex() const { return m_bindingIndex; }

		// Get buffer layout
		FORCEINLINE BufferLayout* getBufferLayout() const { return m_layout; }

		// Get current CPU Buffer Data
		FORCEINLINE BufferData* getBufferData() const { return m_BufferData; }

	protected:
		EBufferType m_bufferType;
		BufferData* m_BufferData;

		bool m_isStatic;

		Int32 m_dataCount;
		Int32 m_bindingIndex;
		BufferLayout* m_layout;

	};
};
#endif // __Z_GPU_BUFFER__

