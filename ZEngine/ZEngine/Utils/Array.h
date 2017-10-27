#ifndef __ZE_ARRAY__
#define __ZE_ARRAY__

#include "ZEngineHelper.h"

#include "../MemoryManagement/Handle.h"
#include "../MemoryManagement/MemoryHelper.h"

#include <cstdlib>

namespace ZE {
	
	template<class T, bool resizable>
	class Array
	{
	public:

		Array() : m_capacity(0), m_length(0)
		{}

		Array(int initialSize) : m_capacity(0), m_length(0)
		{
			reset(initialSize * sizeof(T));
		}

		void reset(int size)
		{
			if (m_handle.isValid())
			{
				m_handle.release();
			}

			m_handle = Handle(size * sizeof(T));
			MemoryHelper::Zero(m_handle.getObject(), m_handle.getCapacity());

			m_capacity = size;
			m_length = 0;
		}

		// doubling the size
		void resize(int size)
		{
			if (!resizable) return;
			
			int sizeToCopy = size > m_capacity ? m_length : size;

			Handle newHandle(size * sizeof(T));
			if (sizeToCopy > 0)
			{
				MemoryHelper::Zero(newHandle.getObject(), newHandle.getCapacity());
				MemoryHelper::Copy(m_handle.getObject(), newHandle.getObject(), sizeToCopy * sizeof(T));
			}

			m_handle.release();
			m_handle = newHandle;
			m_capacity = size;
		}

		T& operator[](int index)
		{
			return get(index);
		}

		T& get(int index)
		{
			return *(T*)((void*)((uintptr_t)m_handle.getObject() + (uintptr_t)(index * sizeof(T))));
		}

		void push_back(T item)
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

		T& pop()
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
				uintptr_t memDest = (uintptr_t)m_handle + (uintptr_t)(index * sizeof(T));
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
				uintptr_t memSrc = (uintptr_t)m_handle + (uintptr_t)(index * sizeof(T));
				uintptr_t memDest = memDest + (uintptr_t) sizeof(T);

				MemoryHelper::Move((void*)memSrc, (void*)memDest, sizeof(T) * (m_length - index));
				m_length++;
			}

			(*this)[index] = item;
		}

		void clear()
		{
			m_length = 0;
		}

		int length() const { return m_length; }
		int size() const { return m_length; }

		int capacity() const { return m_capacity; }

	protected:
		Handle m_handle;
		int m_length;
		int m_capacity;

	};
}

#endif
