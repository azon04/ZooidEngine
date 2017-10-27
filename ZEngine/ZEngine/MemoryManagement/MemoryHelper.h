#ifndef __ZE_MEM_HELPER__
#define __ZE_MEM_HELPER__

namespace ZE
{
	namespace MemoryHelper
	{
		void Copy(void* source, void* dest, size_t size);

		// Move can handle memory overlap when copying, but it's slower than Copy
		void Move(void* source, void* dest, size_t size);

		void Zero(void* dest, size_t size);
	};
};

#endif
