#ifndef __ZE_POOL_ALLOCATOR__
#define __ZE_POOL_ALLOCATOR__

#include "IAllocator.h"
#include <intsafe.h>

namespace ZE {
	class PoolAllocator : public IAllocator
	{

	public:
		PoolAllocator(UINT32 poolSize, size_t itemSize, bool alligned);
		virtual ~PoolAllocator();

		virtual void init(size_t totalSize) override;
		virtual void destroy() override;

		virtual void* allocateMem(size_t size = 0) override;
		void* allocateItem();
		virtual void deallocate(void* _mem) override;

		virtual void* allocateAlign(size_t size, short allign) override;
		virtual void deallocateAlign(void* _mem) override;

		virtual void clear() override;

		void* getBlock(unsigned int block_index);
		unsigned int getIndexOfBlock(void* pMem);

	private:
		void* m_pMemBlock;
		size_t m_totalSize;
		size_t m_itemSize;
		UINT16 m_poolSize;
		UINT8* m_avails;
		bool m_aligned; // NEED to implement

	};
};

#endif
