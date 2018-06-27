#ifndef __ZE_MEMORY_MANAGER_H__
#define __ZE_MEMORY_MANAGER_H__

#define NPOOL 10
#define ADDITIONAL_MAX_POOL 10
#define ALLIGNMENT 16

#include "Platform/Thread.h"

namespace ZE {
	class PoolAllocator;

	class MemoryManager {

	private:
		static MemoryManager* s_instance;

		MemoryManager();
		virtual ~MemoryManager();
	
	protected:
		void* m_pAlocatorsBlock;

		// Put additional pool next to main pool so you can access additional pool using m_pools
		PoolAllocator* m_pools[NPOOL];
		PoolAllocator* m_additionalPools[ADDITIONAL_MAX_POOL];
		unsigned int m_countAddPool;

		void* m_pAdditionalBlocks[ADDITIONAL_MAX_POOL];
		
	public:
		static MemoryManager* Construct();
		static void Deconstruct();
		static MemoryManager* getInstance() { return s_instance; }
		static bool isConstructed() { return s_instance != nullptr; }
		
		void* allocateBlock(size_t size, unsigned int &pool_index, unsigned int &block_index);
		void freeBlock(unsigned int pool_index, unsigned int block_index);
		void* getBlock(unsigned int pool_index, unsigned int block_index);
		void freeBlockAtAddress(void* adds);

		Mutex m_memoryLock;
	};
}
#endif
