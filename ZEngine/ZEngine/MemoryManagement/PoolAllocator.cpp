#include "../ZEngine.h"
#include "PoolAllocator.h"

#include <cstdlib>
#include <cstring>
#include <cstdint>

namespace ZE {

	PoolAllocator::PoolAllocator(UINT32 poolSize, size_t itemSize, bool alligned) 
		: m_poolSize(poolSize), m_itemSize(itemSize), m_pMemBlock(NULL), m_aligned(alligned)
	{}

	PoolAllocator::~PoolAllocator()
	{
	}

	void PoolAllocator::init(size_t totalSize)
	{
		ZASSERT(totalSize / m_itemSize == m_poolSize, "SIZE NOT MATCH");

		m_avails = (UINT8*)malloc(sizeof(UINT8) * m_poolSize);
		memset(m_avails, 1, sizeof(UINT8) * m_poolSize);

		m_totalSize = totalSize;
		m_pMemBlock = malloc(m_totalSize);

	}

	void PoolAllocator::destroy()
	{
		free(m_avails);
		free(m_pMemBlock);
	}
	
	void* PoolAllocator::allocateItem()
	{
		int idx = 0;
		for (; idx < m_poolSize; idx++) 
		{
			if (m_avails[idx] == 1)
				break;
		}

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
		int idx = (reinterpret_cast<uintptr_t>(_mem) - reinterpret_cast<uintptr_t>(_mem)) / m_itemSize;
		m_avails[idx] = 1;
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
			m_avails[idx] = 1;
		}
	}

	void* PoolAllocator::getBlock(unsigned int block_index)
	{
		void* pReturn = nullptr;
		pReturn = (void*)(reinterpret_cast<uintptr_t>(m_pMemBlock) + m_itemSize * block_index);
		return pReturn;
	}

	unsigned int PoolAllocator::getIndexOfBlock(void* pMem)
	{
		uintptr_t diff = reinterpret_cast<uintptr_t>(pMem) - reinterpret_cast<uintptr_t>(m_pMemBlock);
		return diff / m_itemSize;
	}

	void* PoolAllocator::allocateMem(size_t size)
	{
		return allocateItem();
	}
}