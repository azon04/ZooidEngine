#ifndef __ZE_ARRAY__
#define __ZE_ARRAY__

#include "ZEngineHelper.h"

#include "Memory/Handle.h"
#include "Memory/MemoryHelper.h"

#include <cstdlib>
#include <new>

namespace ZE {
	
	template<class T, bool resizable>
	class Array
	{
	public:

		Array() : m_capacity(0), m_length(0), m_handle(Handle())
		{}

		Array(int initialSize) : m_capacity(0), m_length(0), m_handle(Handle())
		{
			reset(initialSize);
		}

		Array(const Array& otherArray)
		{
			if (this == &otherArray)
			{
				return;
			}

			reset(otherArray.capacity());
			m_length = otherArray.length();
			
			for (int i = 0; i < m_length; ++i)
			{
				(*this)[i] = otherArray.getConst(i);
			}
			
		}

		virtual ~Array()
		{
			if (m_handle.isValid())
			{
				T* item = reinterpret_cast<T*> (&get(0));
				for (int i = 0; i < m_capacity; i++)
				{
					T* newItem = item + i;
					newItem->~T();
				}

				m_handle.release();
			}
		}

		void reset(int size)
		{
			if (m_handle.isValid())
			{
				T* item = reinterpret_cast<T*> (&get(0));
				for (int i = 0; i < m_capacity; i++)
				{
					T* newItem = item + i;
					newItem->~T();
				}

				m_handle.release();
			}

			m_handle = Handle(size * sizeof(T));
			MemoryHelper::Zero(m_handle.getObject(), m_handle.getCapacity());
			
			m_capacity = size;
			m_length = 0;

			T* item = reinterpret_cast<T*> (&get(0));
			for (int i = 0; i < m_capacity; i++)
			{
				T* newItem = item + i;
				::new(newItem) T;
			} 
		}

		Array<T, resizable>& operator=(const Array& otherArray)
		{
			if(this == &otherArray) 
			{
				return *this;
			}
			reset(otherArray.capacity());
			m_length = otherArray.length();

			for (int i = 0; i < m_length; ++i)
			{
				(*this)[i] = otherArray.getConst(i);
			}

			return *this;
		}

		// doubling the size
		void resize(int size)
		{
			if (!resizable) return;
			
			int sizeToCopy = size > m_capacity ? m_capacity : size;

			Handle newHandle(size * sizeof(T));
			if (sizeToCopy > 0)
			{
				MemoryHelper::Copy(m_handle.getObject(), newHandle.getObject(), sizeToCopy * sizeof(T));
			}

			m_handle.release();
			m_handle = newHandle;

			T* item = reinterpret_cast<T*> (&get(sizeToCopy));
			for (int i = 0; i < size-sizeToCopy; ++i)
			{
				T* newItem = item + i;
				::new(newItem) T;
			}

			m_capacity = size;
		}

		FORCEINLINE T& operator[](int index)
		{
			return get(index);
		}

		FORCEINLINE T& get(int index)
		{
			return *(T*)((void*)((uintptr_t)m_handle.getObject() + (uintptr_t)(index * sizeof(T))));
		}

		FORCEINLINE T& getConst(int index) const
		{
			return *(T*)((void*)((uintptr_t)m_handle.getObjectConst() + (uintptr_t)(index * sizeof(T))));
		}

		FORCEINLINE T* getPtr(int index)
		{
			(T*)((void*)((uintptr_t)m_handle.getObject() + (uintptr_t)(index * sizeof(T))));
		}

		void push_back(const T& item)
		{
			if (m_length + 1 > m_capacity)
			{
				ZASSERT(resizable, "TRY TO RESIZE AN ARRAY BUT IT'S NOT RESIZABLE");
				if (resizable && m_capacity == 0)
				{
					reset(1);
				}
				else
				{
					resize(m_capacity * 2);
				}
			}

			m_length++;

			(*this)[m_length - 1] = item;
		}

		FORCEINLINE T& pop()
		{
			ZASSERT(m_length > 0, "TRY TO POP AN ARRAY, BUT LENGTH IS 0");
			m_length--;
			return (*this)[m_length];
		}

		void removeAt(int index)
		{
			if (index < m_length-1)
			{
				// Shift Memory
				uintptr_t memDest = (uintptr_t)(m_handle.getObject()) + (uintptr_t)(index * sizeof(T));
				uintptr_t memSrc = memDest + (uintptr_t) sizeof(T);

				MemoryHelper::Move((void*)memSrc, (void*)memDest, sizeof(T) * (m_length - index - 1));
			}
			m_length--;
		}

		void pushAt(int index, T& item)
		{
			if (index >= m_length && index < m_capacity)
			{
				m_length = index + 1;
			}
			else if (index < m_length)
			{
				// unshift the memory
				uintptr_t memSrc = (uintptr_t)(m_handle.getObject()) + (uintptr_t)(index * sizeof(T));
				uintptr_t memDest = memDest + (uintptr_t) sizeof(T);

				MemoryHelper::Move((void*)memSrc, (void*)memDest, sizeof(T) * (m_length - index));
				m_length++;
			}

			(*this)[index] = item;
		}

		FORCEINLINE bool contains(const T& item)
		{
			for (int i = 0; i < m_length; i++)
			{
				if (get(i) == item)
				{
					return true;
				}
			}

			return false;
		}

		FORCEINLINE int firstIndexOf(const T& item)
		{
			for (int i = 0; i < m_length; i++)
			{
				if (get(i) == item)
				{
					return i;
				}
			}

			return -1;
		}

		FORCEINLINE void clear()
		{
			m_length = 0;
		}

		FORCEINLINE int length() const { return m_length; }
		FORCEINLINE int size() const { return m_length; }

		FORCEINLINE int capacity() const { return m_capacity; }

	protected:
		Handle m_handle;
		int m_length;
		int m_capacity;

	};
}

#endif
