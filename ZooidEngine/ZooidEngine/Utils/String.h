#ifndef __ZE_STRING__
#define __ZE_STRING__

#include "../MemoryManagement/Handle.h"

#include "StringFunc.h"

namespace ZE
{
	// Class wrapper for char*
	class String
	{

	public:
		String() 
		{ 
			m_handle = Handle();
		}

		String(const String& other)
		{
			Handle otherHandle = other.getHandle();
			if (m_handle.getCapacity() < otherHandle.getCapacity())
			{
				if (m_handle.isValid())
				{
					m_handle.release();
				}
				m_handle = Handle(otherHandle.getCapacity());
			}

			char* text = m_handle.getObject<char>();
			char* otherText = otherHandle.getObject<char>();

			StringFunc::WriteTo(text, otherText, otherHandle.getCapacity());
		}

		String& operator=(const String& other)
		{
			Handle otherHandle = other.getHandle();
			if (m_handle.getCapacity() < otherHandle.getCapacity())
			{
				if (m_handle.isValid())
				{
					m_handle.release();
				}
				m_handle = Handle(otherHandle.getCapacity());
			}

			char* text = m_handle.getObject<char>();
			char* otherText = otherHandle.getObject<char>();

			StringFunc::WriteTo(text, otherText, otherHandle.getCapacity());

			return *this;
		}

		String(const char* text)
		{
			size_t size = StringFunc::Length(text);
			m_handle = Handle(size);

			char* cText = m_handle.getObject<char>();
			
			StringFunc::WriteTo(cText, text, size);
		}

		~String()
		{
			if (m_handle.isValid())
			{
				m_handle.release();
			}
		}

		String& operator=(const char* text)
		{
			size_t size = StringFunc::Length(text);
			m_handle = Handle(size);

			char* cText = m_handle.getObject<char>();

			StringFunc::WriteTo(cText, text, size);

			return *this;
		}

		char* c_str() { return m_handle.getObject<char>(); }

		Int32 length() { return StringFunc::Length(c_str()); }
		Handle getHandle() const { return m_handle; }

		Handle m_handle;
	};
};
#endif
