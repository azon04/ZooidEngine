#ifndef __ZE_HANDLE_H__
#define __ZE_HANDLE_H__

#include "Utils/ZEngineHelper.h"

#include "MemoryManager.h"

#define INVALID_UINT 0xFFFFFFFF
#define HANDLE_NO_NAME 1

#if !HANDLE_NO_NAME
#include "Utils/StringFunc.h"
#endif

namespace ZE {

	class Handle {

	protected:
		void* pCache;
		unsigned int m_poolIndex;
		unsigned int m_blockIndex;
		size_t m_size;

#if !HANDLE_NO_NAME
		char m_handleName[32];
#endif

	public:

		Handle() :
			m_size(0),
			pCache(nullptr),
			m_poolIndex(INVALID_UINT),
			m_blockIndex(INVALID_UINT)
		{}

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
#if !HANDLE_NO_NAME
			StringFunc::WriteTo(m_handleName, handleName, 32);
#endif
		}

		~Handle() {}

		FORCEINLINE bool isValid() const
		{
			if(pCache == NULL)
				return m_poolIndex != INVALID_UINT && m_blockIndex != INVALID_UINT;
			else 
				return true;
		}
		
		template<typename T>
		FORCEINLINE T* getObject() {
			if (pCache == NULL && (m_poolIndex == INVALID_UINT || m_blockIndex == INVALID_UINT)) {
				pCache = MemoryManager::getInstance()->allocateBlock(m_size, m_poolIndex, m_blockIndex);
			}
			else if (pCache == NULL) {
				pCache = MemoryManager::getInstance()->getBlock(m_poolIndex, m_blockIndex);
			}
			return (T*)pCache;
		}

		FORCEINLINE void* getObject() {
			if (pCache == NULL && (m_poolIndex == INVALID_UINT || m_blockIndex == INVALID_UINT)) {
				pCache = MemoryManager::getInstance()->allocateBlock(m_size, m_poolIndex, m_blockIndex);
			}
			else if (pCache == NULL) {
				pCache = MemoryManager::getInstance()->getBlock(m_poolIndex, m_blockIndex);
			}
			return pCache;
		}

		FORCEINLINE void* getObjectConst() const {
			return pCache;
		}

		void release();

		FORCEINLINE size_t getCapacity() const { return m_size; };
	};

};

// new version using handle
void* operator new(size_t size, ZE::Handle& handle);
void* operator new[](size_t size, ZE::Handle& handle);
void operator delete (void* mem, ZE::Handle& handle);
void operator delete[](void* mem, ZE::Handle& handle);

#endif
