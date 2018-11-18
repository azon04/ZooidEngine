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

		// Implementation from IAllocator
		virtual void init(size_t totalSize) override;
		virtual void destroy() override;

		virtual void* allocateMem(size_t size = 0) override;
		virtual void deallocate(void* _mem) override;

		virtual void* allocateAlign(size_t size, short allign) override;
		virtual void deallocateAlign(void* _mem) override;

		virtual void clear() override;
		// end of IAllocator implementation

		// Allocate an item/block of memory
		virtual void* allocateItem();

		// Pool allocator specific functions

		// get memory block using index
		void* getBlock(unsigned int block_index);

		// get Index of block of memory address
		unsigned int getIndexOfBlock(void* pMem);

		// Allocate free block index
		unsigned int allocateFreeBlockIndex();

		// free block
		void freeBlock(unsigned int block);

		// free block at address
		// return true if the address is part of allocator
		bool freeBlockAtAddress(void* adds);

		// get number of free block
		int getCountFreeBlock() const { return m_freeBlock; }

		// check if the allocator is empty
		bool isEmpty() const { return m_freeBlock == m_poolSize; }

		// Construct PoolAllocator using memory address, size of block and number of block needed
		// pMem should be memory address allocate with size of CalculateSizeMem return value
		static PoolAllocator* ConstructFromMem(void* pMem, size_t itemSize, unsigned int blockCount, bool bAlign, int alignment);

		// Calculate size of memory needed to create pool alocator with itemSize and number of block needed
		static size_t CalculateSizeMem(size_t itemSize, unsigned int blockCount, bool bAlign, int alignment);

		size_t getItemSize() const
		{
			return m_itemSize;
		}

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
