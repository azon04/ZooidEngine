#include "MemoryManager.h"
#include "PoolAllocator.h"
#include "Utils/Debug.h"
#include "Logging/Log.h"

#include <cstdlib>

#define MAX_COUNT_ADD_POOL 1024

// Max Memory in additional pool is gonna be 2MB
#define MAX_MEM_ADD_POOL (2 * 1024 * 1024)

static unsigned int poolConfig[NPOOL][2] =
{
	{32, 1024}, // 32 * 1024 =~ 32 kb
	{64, 1024}, // 64 * 1024 =~ 64 kb 
	{128, 1024}, // 128 * 1024 =~ 128 kb
	{256, 1024}, // 256 * 1024 =~ 256 kb
	{512, 1024}, // 512 * 1024 =~ 512 kb
	{1024, 1024}, // 1024 * 1024 =~ 1 MB
	{2048, 1024}, // 2048 * 1024 =~ 2 MB
	{139272, 1}, // Draw list need this big of memoryblock
	{278544, 1}, // Draw list for 64bit machine
	{767360, 2} // 767360 * 2 =~ 1.5 MB; Block for loading big assets data
};

namespace ZE {

	MemoryManager* MemoryManager::s_instance = nullptr;

	MemoryManager::MemoryManager() :
		m_pAlocatorsBlock(nullptr), m_countAddPool(0)
	{}

	MemoryManager::~MemoryManager()
	{
		if (m_pAlocatorsBlock) 
		{
			free(m_pAlocatorsBlock);
		}

		for (int i = 0; i < m_countAddPool; i++)
		{
			free(m_pAdditionalBlocks[i]);
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

		ZELOG(LOG_MEMORY, Log, "Total Mem Size : %d bytes ~ %f MB", totalSize, totalSize / (1024.0f * 1024.0f));
		
		// Allocate the memory needed for pool
		s_instance->m_pAlocatorsBlock = (void*) malloc(totalSize);
		
		// #TODO align memory first and construct pools
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
		s_instance = nullptr;
	}

	void* MemoryManager::allocateBlock(size_t size, unsigned int &pool_index, unsigned int &block_index)
	{
		m_memoryLock.lock();

		ZELOG(LOG_MEMORY, Log, "Allocate memory for %d bytes", size);

		// Check the matched size in pools
		int index = 0;
		while (index < NPOOL && poolConfig[index][0] < size)
		{
			ZELOG(LOG_MEMORY, Log, "Cant fit in %d bytes", poolConfig[index][0]);
			index++;
		}

		ZASSERT(index < NPOOL, "Can't find pool for the requested size");

		while (index < NPOOL && m_pools[index]->getCountFreeBlock() == 0)
		{
			ZELOG(LOG_MEMORY, Log, "Cant find free block in %d bytes", poolConfig[index][0]);
			index++;
		}

		if (index >= NPOOL) // Looking inside the additional pools
		{
			while (index < m_countAddPool + NPOOL && size > m_additionalPools[index - NPOOL]->getItemSize() && !m_additionalPools[index - NPOOL]->isEmpty() && m_additionalPools[index - NPOOL]->getCountFreeBlock() == 0)
			{
				index++;
			}

			if (index >= (NPOOL + m_countAddPool) && m_countAddPool < ADDITIONAL_MAX_POOL)
			{
				int poolCount = (MAX_MEM_ADD_POOL / size) < MAX_COUNT_ADD_POOL ? (MAX_MEM_ADD_POOL / size) : MAX_MEM_ADD_POOL;
				size_t totalSize = PoolAllocator::calculateSizeMem(size, poolCount);
				m_pAdditionalBlocks[m_countAddPool] = (void*)malloc(totalSize);
				m_additionalPools[m_countAddPool] = PoolAllocator::constructFromMem(m_pAdditionalBlocks[m_countAddPool], size, poolCount);
				m_countAddPool++;

				ZELOG(LOG_MEMORY, Log, "Created Additional Memory Pool for %d bytes with %d blocks", size, poolCount);
			}
			else if (m_additionalPools[index - NPOOL]->isEmpty() && size > m_additionalPools[index - NPOOL]->getItemSize()) // For empty additional block
			{
				int poolCount = (MAX_MEM_ADD_POOL / size) < MAX_COUNT_ADD_POOL ? (MAX_MEM_ADD_POOL / size) : MAX_MEM_ADD_POOL;
				size_t totalSize = PoolAllocator::calculateSizeMem(size, poolCount);
				// Free Empty additional block
				free(m_pAdditionalBlocks[index - NPOOL]);
				m_pAdditionalBlocks[index - NPOOL] = (void*)malloc(totalSize);

				m_additionalPools[index - NPOOL] = PoolAllocator::constructFromMem(m_pAdditionalBlocks[m_countAddPool], size, poolCount);

				ZELOG(LOG_MEMORY, Log, "Created Additional Memory Pool for %d bytes with %d blocks replacing block on index %d", size, poolCount, index);
			}
		}

		ZASSERT(index < (NPOOL + m_countAddPool), "Can't find available free block");
		ZASSERT(m_pools[index]->getCountFreeBlock() > 0, "Need more size for pool");
		
		// Allocate in memory
		pool_index = index;
		block_index = m_pools[index]->allocateFreeBlockIndex();

		m_memoryLock.unlock();
		return m_pools[pool_index]->getBlock(block_index);
	}

	void MemoryManager::freeBlock(unsigned int pool_index, unsigned int block_index)
	{
		m_memoryLock.lock();
		ZASSERT(pool_index < NPOOL + m_countAddPool, "Pool index out of bound");
		m_pools[pool_index]->freeBlock(block_index);
		m_memoryLock.unlock();
	}

	void* MemoryManager::getBlock(unsigned int pool_index, unsigned int block_index)
	{
		ZASSERT(pool_index < NPOOL + m_countAddPool, "Index Pool out of bound");
		void* pReturnMem = m_pools[pool_index]->getBlock(block_index);
		return pReturnMem;
	}

	void MemoryManager::freeBlockAtAddress(void* adds)
	{
		for (int i = 0; i < NPOOL; ++i)
		{
			if (m_pools[i]->freeBlockAtAddress(adds))
				return;
		}
	}

}