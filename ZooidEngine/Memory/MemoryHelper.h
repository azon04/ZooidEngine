#ifndef __ZE_MEM_HELPER__
#define __ZE_MEM_HELPER__

#include "Utils/Macros.h"
#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	namespace MemoryHelper
	{

		// Copy bit from source to dest
		void Copy(void* source, void* dest, size_t size);

		// Move can handle memory overlap when copying, but it's slower than Copy
		void Move(void* source, void* dest, size_t size);

		// Zeroed the dest memory
		void Zero(void* dest, size_t size);

		// Copy texture memory to another texture memory
		void CopyTexture(void* destTexMemory, Int32 destWidth, Int32 destHeight, Int32 targetX, Int32 targetY, void* srcTexMemory, Int32 srcWidth, Int32 srcHeight, size_t itemSize);

	};
};

#endif
