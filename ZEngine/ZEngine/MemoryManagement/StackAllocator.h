#ifndef __ZE_STACK_ALLOCATOR__
#define __ZE_STACK_ALLOCATOR__

#include "IAllocator.h"

namespace ZE {
	class StackAllocator : public IAllocator
	{
		
	public:
		StackAllocator();
		virtual ~StackAllocator();

		virtual void init(size_t totalSize) override;
		virtual void destroy() override;

		virtual void* allocateMem(size_t size) override;
		virtual void deallocate(void* _mem) override;

		virtual void* allocateAlign(size_t size, short allign) override;
		virtual void deallocateAlign(void* _mem) override;

		virtual void clear() override;

		unsigned int getCurrentMarker();
		void setCurrentMarker(unsigned int marker_location);

	private:
		void* m_pMemBlock;
		size_t m_totalSize;
		void* m_pCurrentBlock;
	};
};
#endif
