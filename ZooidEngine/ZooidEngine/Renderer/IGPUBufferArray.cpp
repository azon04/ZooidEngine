#include "IGPUBufferArray.h"


namespace ZE 
{

	IGPUBufferArray::IGPUBufferArray()
	{

	}

	IGPUBufferArray::~IGPUBufferArray()
	{
		release();
	}

}