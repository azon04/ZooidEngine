#ifndef __ZE_MEMORY_MANAGER_H__
#define __ZE_MEMORY_MANAGER_H__

#define NPOOL 7
#define ALLIGNMENT 16

namespace ZE {
	class PoolAllocator;

	class MemoryManager {

	private:
		static MemoryManager* s_instance;
	
	protected:
		void* m_pAlocatorsBlock;
		PoolAllocator* m_pools[NPOOL];
	
	public:
		MemoryManager();
		virtual ~MemoryManager();

		static MemoryManager* Construct();
		static void Deconstruct();
		static MemoryManager* getInstance() { return s_instance; }
		
		void* allocateBlock(size_t size, unsigned int &pool_index, unsigned int &block_index);
		void freeBlock(unsigned int pool_index, unsigned int block_index);
		void* getBlock(unsigned int pool_index, unsigned int block_index);
	};
}
#endif
