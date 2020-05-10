#ifndef __ZE_STRING_NAME_H__
#define __ZE_STRING_NAME_H__

#include "Memory/Handle.h"
#include "Memory/MemoryHelper.h"

#include "String.h"

namespace ZE
{
	// Optimized version of String. This is the best for comparison, since it's using the string has
	class StringName
	{
	public:

		StringName()
		{
			m_handle = Handle();
			m_hash = 0;
		}

		StringName(const char* string, Int32 len)
		{
			if (MemoryManager::GetInstance())
			{
				m_handle = Handle(len + 1);
				StringFunc::WriteTo(m_handle.getObject<char>(), string, len);
				m_handle.getObject<char>()[len - 1] = '\0';
			}
			else
			{
				m_cache = string;
			}

			m_hash = StringFunc::Hash(string, len);
		}

		StringName(const char* string)
			: StringName(string, StringFunc::Length(string))
		{}

		String toString()
		{
			if (m_handle.isValid())
			{
				String result = m_handle.getObject<char>();
				return result;
			}
			return m_cache;
		}

		bool operator==(const StringName& name) const
		{
			return m_hash == name.m_hash;
		}

		bool operator!=(const StringName& name) const
		{
			return m_hash != name.m_hash;
		}

	protected:

		Handle m_handle;
		UInt32 m_hash;
		const char* m_cache = NULL;
	};
}
#endif
