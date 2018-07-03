#include "IGPUBufferData.h"

#include <GL/glew.h>

namespace ZE 
{

	IGPUBufferData::~IGPUBufferData()
	{
		release();
	}

	void IGPUBufferData::FromBufferData(BufferData* _bufferData)
	{
		m_bufferType = _bufferData->m_type;
		m_BufferData = _bufferData;
		m_dataCount = _bufferData->m_count;

		if (m_bufferType == VERTEX_BUFFER)
		{
			SetupLayout(BufferLayoutManager::GetInstance()->getBufferLayoutByFormat(_bufferData->m_bufferLayout));
		}
	}

	void IGPUBufferData::SetupLayout(BufferLayout* _layouts)
	{
		this->m_layout = _layouts;
	}
}
