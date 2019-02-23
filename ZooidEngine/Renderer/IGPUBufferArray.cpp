#include "IGPUBufferArray.h"


namespace ZE 
{

	IGPUBufferArray::IGPUBufferArray()
	{
		m_indexBuffer = nullptr;
	}

	IGPUBufferArray::~IGPUBufferArray()
	{
		release();
	}

}