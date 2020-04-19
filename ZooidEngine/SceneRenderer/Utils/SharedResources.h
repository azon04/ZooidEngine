#ifndef __ZE_UTIL_SHARED_RESOURCE_H__
#define __ZE_UTIL_SHARED_RESOURCE_H__

#include "Common//SingletonClass.h"

namespace ZE
{
	class IGPUBufferArray;

	class UtilShaderResources : public Singleton<UtilShaderResources>
	{
	public:
		static IGPUBufferArray* GetCubeVerticesOnly();
	};
};
#endif
