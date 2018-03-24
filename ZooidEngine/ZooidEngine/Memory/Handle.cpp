#include "Handle.h"


namespace ZE {

	void Handle::release()
	{
		if (isValid() && MemoryManager::getInstance())
		{
			MemoryManager::getInstance()->freeBlock(m_poolIndex, m_blockIndex);
		}
		m_poolIndex = INVALID_UINT;
		m_blockIndex = INVALID_UINT;
		pCache = nullptr;
	}

}

void* operator new(size_t size, ZE::Handle& handle)
{
	return handle.getObject();
}

void* operator new[](size_t size, ZE::Handle& handle)
{
	return handle.getObject();
}

void operator delete(void* mem, ZE::Handle& handle)
{
	if (handle.isValid())
	{
		handle.release();
	}
	else
	{
		ZE::MemoryManager::getInstance()->freeBlockAtAddress(mem);
	}
}

void operator delete[](void* mem, ZE::Handle& handle)
{
	if (handle.isValid())
	{
		handle.release();
	}
	else
	{
		ZE::MemoryManager::getInstance()->freeBlockAtAddress(mem);
	}
}