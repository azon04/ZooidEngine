#include "MemoryManager.h"
#include "../ZEngine.h"
#include "PoolAllocator.h"
#include <cstdlib>
#include "../Utils/Debug.h"

static unsigned int poolConfig[NPOOL][2] =
{
	{32, 1024}, // 32 * 1024 =~ 32 kb
	{64, 1024}, // 64 * 1024 =~ 64 kb 
	{128, 1024}, // 128 * 1024 =~ 128 kb
	{256, 1024}, // 256 * 1024 =~ 256 kb
	{512, 1024}, // 512 * 1024 =~ 512 kb
	{1024, 1024}, // 1024 * 1024 =~ 1 MB
	{16384, 1}, // Draw list need this big of memoryblock
	{24584, 1} // Draw list for 64bit machine
};

namespace ZE {

	MemoryManager* MemoryManager::s_instance = nullptr;

	MemoryManager::MemoryManager() :
		m_pAlocatorsBlock(nullptr)
	{}

	MemoryManager::~MemoryManager()
	{
		if (m_pAlocatorsBlock) 
		{
			free(m_pAlocatorsBlock);
		}
	}

	MemoryManager* MemoryManager::Construct()
	{
		s_instance = new MemoryManager();

		// Calculate total memory needed
		size_t totalSize = 0;
		for (unsigned int i = 0; i < NPOOL; i++)
		{
			totalSize += PoolAllocator::calculateSizeMem(poolConfig[i][0], poolConfig[i][1]) + ALLIGNMENT;
		}
		ZEINFO("Total Mem Size : %d bytes ~ %f MB", totalSize, totalSize / (1024.0f * 1024.0f));

		// Allocate the memory needed for pool
		s_instance->m_pAlocatorsBlock = (void*) malloc(totalSize);
		
		// TO DO align memory first and construct pools
		void* pMem = s_instance->m_pAlocatorsBlock;
		for (unsigned int i = 0; i < NPOOL; i++)
		{
			size_t mask = ALLIGNMENT - 1;
			uintptr_t misaligned = ((uintptr_t)(pMem)& mask);
			ptrdiff_t adjustment = ALLIGNMENT - misaligned;

			pMem = (void*)((uintptr_t)pMem + adjustment);
			s_instance->m_pools[i] = PoolAllocator::constructFromMem(pMem, poolConfig[i][0], poolConfig[i][1]);
			pMem = (void*) ((uintptr_t) pMem + PoolAllocator::calculateSizeMem(poolConfig[i][0], poolConfig[i][1]));
		}

		return s_instance;
	}

	void MemoryManager::Deconstruct()
	{
		delete s_instance;
	}

	void* MemoryManager::allocateBlock(size_t size, unsigned int &pool_index, unsigned int &block_index)
	{
		ZEINFO("Allocate memory for %d bytes", size);

		// Check the matched size in pools
		int index = 0;
		while (index < NPOOL && poolConfig[index][0] < size)
		{
			ZEINFO("Cant fit in %d bytes", poolConfig[index][0]);
			index++;
		}

		ZASSERT(index < NPOOL, "Can't find pool for the requested size");

		while (index < NPOOL && m_pools[index]->getCountFreeBlock() == 0)
		{
			ZEINFO("Cant find free block in %d bytes", poolConfig[index][0]);
			index++;
		}

		ZASSERT(index < NPOOL, "Can't find available available free block");
		ZASSERT(m_pools[index]->getCountFreeBlock() > 0, "Need more size for pool");
		
		// Allocate in memory
		pool_index = index;
		block_index = m_pools[index]->allocateFreeBlockIndex();

		return m_pools[pool_index]->getBlock(block_index);
	}

	void MemoryManager::freeBlock(unsigned int pool_index, unsigned int block_index)
	{
		ZASSERT(pool_index < NPOOL, "Pool index out of bound");
		m_pools[pool_index]->deallocate(m_pools[pool_index]->getBlock(block_index));
	}

	void* MemoryManager::getBlock(unsigned int pool_index, unsigned int block_index)
	{
		ZASSERT(pool_index < NPOOL, "Index Pool out of bound");
		void* pReturnMem = m_pools[pool_index]->getBlock(block_index);
		return pReturnMem;
	}

}