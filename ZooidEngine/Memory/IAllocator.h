#ifndef __ZE_INTERFACE_ALLOCATOR__
#define __ZE_INTERFACE_ALLOCATOR__

namespace ZE 
{

	class IAllocator 
	{

	public:
		virtual void init(size_t totalsize) = 0;
		virtual void destroy() = 0;

		virtual void* allocateMem(size_t size) = 0;
		virtual void deallocate(void* _mem) = 0;

		virtual void* allocateAlign(size_t size, short allign) = 0;
		virtual void deallocateAlign(void* _mem) = 0;

		virtual void clear() = 0;
	};
};
#endif
