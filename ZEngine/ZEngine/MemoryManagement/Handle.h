#ifndef __ZE_HANDLE_H__
#define __ZE_HANDLE_H__

#include "../GameObjectModel/Object.h"
#include "MemoryManager.h"
#include "../Utils/StringFunc.h"

#define INVALID_UINT 0xFFFFFFFF

namespace ZE {

	class Handle {

	protected:
		void* pCache;
		unsigned int m_poolIndex;
		unsigned int m_blockIndex;
		size_t m_size;
		char m_handleName[32];

	public:
		Handle(size_t size) :
			m_size(size),
			pCache(nullptr),
			m_poolIndex(INVALID_UINT),
			m_blockIndex(INVALID_UINT)
		{}

		Handle(const char* handleName, size_t size) :
			m_size(size),
			pCache(nullptr),
			m_poolIndex(INVALID_UINT),
			m_blockIndex(INVALID_UINT)
		{
			StringFunc::WriteTo(m_handleName, handleName, 32);
		}

		~Handle() {}

		bool isValid() 
		{
			if(pCache == NULL)
				return m_poolIndex == INVALID_UINT || m_blockIndex == INVALID_UINT;
			else 
				return true;
		}
		
		template<typename T>
		T* getObject() {
			if (pCache == NULL && (m_poolIndex == INVALID_UINT || m_blockIndex == INVALID_UINT)) {
				pCache = MemoryManager::getInstance()->allocateBlock(m_size, m_poolIndex, m_blockIndex);
			}
			else if (pCache == NULL) {
				pCache = MemoryManager::getInstance()->getBlock(m_poolIndex, m_blockIndex);
			}
			return (T*)pCache;
		}

		void* getObject() {
			if (pCache == NULL && (m_poolIndex == INVALID_UINT || m_blockIndex == INVALID_UINT)) {
				pCache = MemoryManager::getInstance()->allocateBlock(m_size, m_poolIndex, m_blockIndex);
			}
			else if (pCache == NULL) {
				pCache = MemoryManager::getInstance()->getBlock(m_poolIndex, m_blockIndex);
			}
			return pCache;
		}

		void release();
	};

};

// new version using handle
void* operator new(size_t size, ZE::Handle& handle);
void* operator new[](size_t size, ZE::Handle& handle);
void operator delete (void* mem, ZE::Handle& handle);
void operator delete[](void* mem, ZE::Handle& handle);

#endif
