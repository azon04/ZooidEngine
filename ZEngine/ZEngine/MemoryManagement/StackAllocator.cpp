#include "StackAllocator.h"

#include <cstdlib>
#include <cstdint>

namespace ZE {

	StackAllocator::StackAllocator()
	{
		m_MemBlock = NULL;
		m_currentBlock = NULL;
		m_totalSize = 0;
	}

	void StackAllocator::init(size_t totalSize)
	{
		m_totalSize = totalSize;
		m_MemBlock = malloc(m_totalSize);
		m_currentBlock = m_MemBlock;
	}

	void StackAllocator::destroy()
	{
		//memset(m_MemBlock, 0, m_totalSize);
		free(m_MemBlock);
		m_MemBlock = NULL;
		m_currentBlock = NULL;
	}

	void* StackAllocator::allocateMem(size_t size)
	{
		if ((int)m_currentBlock + size - (int)m_MemBlock > m_totalSize)
			return NULL;

		void* mem = m_currentBlock;
		m_currentBlock = (void*)((uintptr_t)m_currentBlock + size);
		return mem;
	}

	void StackAllocator::deallocate(void* _mem)
	{
		// NOT IMPLEMENTED
	}

	void* StackAllocator::allocateAlign(size_t size, short allign)
	{
		size_t totalSize = size + allign;
		uintptr_t rawMem = reinterpret_cast<uintptr_t>(allocateMem(totalSize));
		
		size_t mask = allign - 1;
		uintptr_t misaligned = (rawMem & mask);
		ptrdiff_t adjustment = allign - misaligned;

		uintptr_t alignedAddress = (rawMem + adjustment);

		uint8_t* pAlignedMem = reinterpret_cast<uint8_t*>(alignedAddress);
		pAlignedMem[-1] = static_cast<uint8_t>(adjustment);

		return (void*)(pAlignedMem);
	}

	void StackAllocator::deallocateAlign(void* _mem)
	{
		// NOT IMPLEMENTED FOR STACK ALLOCATOR
	}

	void StackAllocator::clear()
	{
		m_currentBlock = m_MemBlock;
	}

	unsigned int StackAllocator::getCurrentMarker()
	{
		return (reinterpret_cast<uintptr_t>(m_currentBlock) - reinterpret_cast<uintptr_t>(m_MemBlock));
	}

	void StackAllocator::setCurrentMarker(unsigned int marker_location)
	{
		m_currentBlock =  (void*)(reinterpret_cast<uintptr_t>(m_MemBlock) + marker_location);
	}

	StackAllocator::~StackAllocator()
	{
		if (m_MemBlock) {
			destroy();
		}
	}

}