#ifndef __ZE_MEMORY_MANAGER_H__
#define __ZE_MEMORY_MANAGER_H__

#define NPOOL 10
#define ADDITIONAL_MAX_POOL 30
#define ALLIGNMENT 16

#include "Platform/Thread.h"

namespace ZE 
{

	class PoolAllocator;

	class MemoryManager 
	{

	private:
		MemoryManager();
		virtual ~MemoryManager();

	public:
		// Construct memory manager instance
		static MemoryManager* Construct();
		
		// Destroy memory manager instance
		static void Deconstruct();

		// get memory manager instance
		static MemoryManager* GetInstance() { return s_instance; }

		// Check if the memory manager is constructed
		static bool IsConstructed() { return s_instance != nullptr; }
		
		// Allocate block memory with size;
		// output: pool index and block index
		void* allocateBlock(size_t size, unsigned int &pool_index, unsigned int &block_index);

		// Free block memory in block index at pool index
		void freeBlock(unsigned int pool_index, unsigned int block_index);

		// Get block memory at pool_index and block_index
		void* getBlock(unsigned int pool_index, unsigned int block_index);

		// Free block with address
		void freeBlockAtAddress(void* adds);

	private:
		static MemoryManager* s_instance;
		Mutex m_memoryLock;

	protected:
		void* m_pAlocatorsBlock;

		// Put additional pool next to main pool so you can access additional pool using m_pools
		PoolAllocator* m_pools[NPOOL];
		PoolAllocator* m_additionalPools[ADDITIONAL_MAX_POOL];
		unsigned int m_countAddPool;

		void* m_pAdditionalBlocks[ADDITIONAL_MAX_POOL];

	};
}
#endif
