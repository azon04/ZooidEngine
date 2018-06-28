#ifndef __ZE_MEM_HELPER__
#define __ZE_MEM_HELPER__

#include "Utils/Macros.h"

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
	};
};

#endif
