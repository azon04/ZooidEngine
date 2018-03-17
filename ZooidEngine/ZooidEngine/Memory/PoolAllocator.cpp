#include "PoolAllocator.h"

#include "Utils/ZEngineHelper.h"

#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <new>

namespace ZE {

	PoolAllocator::PoolAllocator(UINT32 poolSize, size_t itemSize, bool alligned) 
		: m_poolSize(poolSize), m_itemSize(itemSize), m_pMemBlock(NULL), m_aligned(alligned)
		, m_bNeedToFree(true)
	{}

	PoolAllocator::~PoolAllocator()
	{
	}

	void PoolAllocator::init(size_t totalSize)
	{
		ZASSERT(totalSize / m_itemSize == m_poolSize, "SIZE NOT MATCH");

		*m_avails = (unsigned int)malloc(sizeof(unsigned int) * m_poolSize);
		for (unsigned int i = 0; i < m_poolSize; i++)
		{
			m_avails[i] = i;
		}

		m_totalSize = totalSize;

		// #TODO need to align
		m_pMemBlock = malloc(m_totalSize);

	}

	void PoolAllocator::destroy()
	{
		if (m_bNeedToFree)
		{
			free(m_avails);
			free(m_pMemBlock);
		}
	}
	
	void* PoolAllocator::allocateItem()
	{
		unsigned int idx = allocateFreeBlockIndex();

		if (idx < m_poolSize) 
		{
			m_avails[idx] = 0;
			void* mem = (void*)(reinterpret_cast<uintptr_t>(m_pMemBlock) + m_itemSize * idx);
			return mem;
		}
		else {
			return NULL;
		}
	}

	void PoolAllocator::deallocate(void* _mem)
	{
		// Mark as avail
		int idx = (reinterpret_cast<uintptr_t>(_mem) - reinterpret_cast<uintptr_t>(m_pMemBlock)) / m_itemSize;
		m_avails[m_freeBlock++] = idx;
	}

	void* PoolAllocator::allocateAlign(size_t size, short allign)
	{
		// Not Implemented for POOL yet
		return NULL;
	}

	void PoolAllocator::deallocateAlign(void* _mem)
	{
		// Not Implemented for POOL
	}

	void PoolAllocator::clear()
	{
		for (int idx = 0; idx < m_poolSize; idx++) {
			m_avails[idx] = idx;
		}
	}

	void* PoolAllocator::getBlock(unsigned int block_index)
	{
		void* pReturn = nullptr;
		pReturn = (void*)(reinterpret_cast<uintptr_t>(m_pMemBlock) + m_itemSize * block_index);
		return pReturn;
	}

	unsigned int PoolAllocator::allocateFreeBlockIndex()
	{
		return m_avails[--m_freeBlock];
	}

	unsigned int PoolAllocator::getIndexOfBlock(void* pMem)
	{
		uintptr_t diff = reinterpret_cast<uintptr_t>(pMem) - reinterpret_cast<uintptr_t>(m_pMemBlock);
		return diff / m_itemSize;
	}

	PoolAllocator* PoolAllocator::constructFromMem(void* pMem, size_t itemSize, unsigned int blockCount)
	{
		// NOTE : This allocation method, need to assign the vtable.
		// Not to do that I need to make this class have no vtable.
		// which mean no virtual method.

		// Using new(pMem) it will handle vtable allocation for us
		PoolAllocator* poolAllocator =  new(pMem) PoolAllocator(blockCount, itemSize, false);
		poolAllocator->m_totalSize = itemSize * blockCount;
		poolAllocator->m_itemSize = itemSize;
		poolAllocator->m_poolSize = blockCount;
		poolAllocator->m_aligned = false;
		poolAllocator->m_bNeedToFree = false;
		poolAllocator->m_freeBlock = blockCount;

		for (unsigned int i = 0; i < blockCount; i++) 
		{
			poolAllocator->m_avails[i] = i;
		}

		poolAllocator->m_pMemBlock = (void*)((uintptr_t)poolAllocator->m_avails + (uintptr_t)(blockCount * sizeof(unsigned int)));
		// #TODO need to align

		return poolAllocator;
	}

	size_t PoolAllocator::calculateSizeMem(size_t itemSize, unsigned int blockCount)
	{
		size_t size = sizeof(PoolAllocator);
		size += sizeof(unsigned int) * blockCount;
		// Need Align

		// #TODO need to align

		size += itemSize * blockCount;
		return size;
	}

	void* PoolAllocator::allocateMem(size_t size)
	{
		return allocateItem();
	}

	void PoolAllocator::freeBlock(unsigned int block)
	{
		m_avails[m_freeBlock++] = block;
	}

}