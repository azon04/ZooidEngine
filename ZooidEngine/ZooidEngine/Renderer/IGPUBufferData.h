#ifndef __Z_GPU_BUFFER__
#define __Z_GPU_BUFFER__

#include "Utils/PrimitiveTypes.h"

#include "BufferData.h"
#include "BufferLayout.h"

#define MAX_LAYOUT 16
namespace ZE {

class IGPUBufferData {

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

	virtual void FromBufferData(BufferData* _bufferData);
	void SetupLayout(BufferLayout* _layouts);

	virtual void Bind() = 0;
	virtual void UnBind() = 0;
	virtual void release() {};

	BufferType m_bufferType;
	BufferData* m_BufferData;

	bool m_isStatic;

	ZE::Int32 m_dataCount;
	ZE::Int32 m_bindingIndex;
	BufferLayout* m_layout;

};
};
#endif // __Z_GPU_BUFFER__

