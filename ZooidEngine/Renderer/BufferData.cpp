#include "BufferData.h"

namespace ZE
{
	void BufferData::setData(Handle hData, unsigned int sizePerItem, unsigned int count /*= 1*/)
	{
		m_data = hData.getObject();
		m_hData = hData;
		m_size = sizePerItem * count;
		m_count = count;
	}
}
