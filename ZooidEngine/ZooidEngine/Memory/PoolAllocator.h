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

		virtual void init(size_t totalSize);
		virtual void destroy();

		virtual void* allocateMem(size_t size = 0);
		virtual void* allocateItem();
		virtual void deallocate(void* _mem);

		virtual void* allocateAlign(size_t size, short allign);
		virtual void deallocateAlign(void* _mem);

		virtual void clear();

		// Pool allocator specific function
		void* getBlock(unsigned int block_index);
		unsigned int getIndexOfBlock(void* pMem);
		unsigned int allocateFreeBlockIndex();
		void freeBlock(unsigned int block);
		int getCountFreeBlock() const { return m_freeBlock; }

		static PoolAllocator* constructFromMem(void* pMem, size_t itemSize, unsigned int blockCount);
		static size_t calculateSizeMem(size_t itemSize, unsigned int blockCount);

	private:
		void* m_pMemBlock;
		size_t m_totalSize;
		size_t m_itemSize;
		UINT16 m_poolSize;
		size_t m_freeBlock;
		bool m_aligned; // NEED to implement
		bool m_bNeedToFree;
		unsigned int m_avails[1];
	};
};

#endif
