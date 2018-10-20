#include "IGPUBufferData.h"
#include "BufferData.h"

#include "Renderer/BufferLayout.h"

#include <GL/glew.h>

namespace ZE 
{

	IGPUBufferData::~IGPUBufferData()
	{
		release();
	}

	void IGPUBufferData::FromBufferData(BufferData* _bufferData)
	{
		m_bufferType = _bufferData->getBufferType();
		m_BufferData = _bufferData;
		m_dataCount = _bufferData->getCount();

		if (m_bufferType == VERTEX_BUFFER)
		{
			SetupLayout(BufferLayoutManager::GetInstance()->getBufferLayoutByFormat(_bufferData->getBufferLayout()));
		}
	}

	void IGPUBufferData::SetupLayout(BufferLayout* _layouts)
	{
		this->m_layout = _layouts;
	}
}
