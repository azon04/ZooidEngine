#include "MemoryManager.h"
#include "../ZEngine.h"
#include "PoolAllocator.h"
#include <cstdlib>

namespace ZE {

	MemoryManager* MemoryManager::s_instance = nullptr;

	MemoryManager::MemoryManager() :
		m_pAlocatorsBlock(nullptr)
	{

	}

	MemoryManager::~MemoryManager()
	{
		if (m_pAlocatorsBlock) {
			free(m_pAlocatorsBlock);
		}
	}

	MemoryManager* MemoryManager::Construct()
	{
		s_instance = new MemoryManager();

		// Calculate total memory needed

		// Allocate the memory needed for pool
		
		return s_instance;
	}

	void MemoryManager::Deconstruct()
	{
		delete s_instance;
	}

	void* MemoryManager::allocateBlock(size_t size, unsigned int &pool_index, unsigned int &block_index)
	{
		ZASSERT(pool_index < NPOOL, "Index Pool out of bound");

		// Check the matched size in pools

		// Allocate in memory

		return nullptr;
	}

	void MemoryManager::freeBlock(unsigned int pool_index, unsigned int block_index)
	{
		ZASSERT(pool_index < NPOOL, "Index Pool out of bound");
		m_pools[pool_index]->deallocate(m_pools[pool_index]->getBlock(block_index));
	}

	void* MemoryManager::getBlock(unsigned int pool_index, unsigned int block_index)
	{
		ZASSERT(pool_index < NPOOL, "Index Pool out of bound");
		void* pReturnMem = m_pools[pool_index]->getBlock(block_index);
		return pReturnMem;
	}

}