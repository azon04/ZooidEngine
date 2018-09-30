#ifndef __Z_BUFFER_DATA__
#define __Z_BUFFER_DATA__

#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"
#include "Enums.h"

namespace ZE 
{

	class BufferData
	{

	public:
		BufferData(EBufferType type) : m_type(type), m_bStatic(true)
		{}

		virtual ~BufferData() {}

		void SetData(void* Data, unsigned int sizePerItem, unsigned int count = 1)
		{
			m_data = Data;
			m_size = sizePerItem * count;
			m_count = count;
		}

		void setData(Handle hData, unsigned int sizePerItem, unsigned int count = 1);

		void setBufferLayout(Int32 _bufferLayout) { m_bufferLayout = _bufferLayout; }

		void* getData() const { return m_data; }
		Handle getHandle() const { return m_hData; }
		Int32 getBufferLayout() const { return m_bufferLayout; }
		UInt32 getCount() const { return m_count; }
		UInt32 getSize() const { return m_size; }
		EBufferType getBufferType() const { return m_type; }
		void setStaticBuffer(bool bStatic) { m_bStatic = bStatic; }
		bool isStaticBuffer() const { return m_bStatic; }


	private:
		Handle m_hData;
		void* m_data;
		Int32 m_bufferLayout;
		UInt32 m_count;
		UInt32 m_size;
		EBufferType m_type;
		bool m_bStatic;
	};

};
#endif
