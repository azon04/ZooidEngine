#include "DrawList.h"

#include "BufferManager.h"
namespace ZE {

	void DrawList::Reset()
	{
		for (int i = 0; i < m_size; i++)
		{
			m_drawList[i].Reset();
		}

		m_size = 0;
	}

	void DrawList::Setup()
	{
		BufferManager* pBufferManager = BufferManager::getInstance();
		
		Handle hConstantBufferData("Constant Buffer Data", sizeof(BufferData));
		m_constantBufferData = new(hConstantBufferData) BufferData(UNIFORM_BUFFER);
		m_constantBufferData->SetData(&m_shaderData, sizeof(ShaderData));

		m_mainConstantBuffer = pBufferManager->createGPUBufferFromBuffer(m_constantBufferData, false, true);
		m_mainConstantBuffer->m_bindingIndex = 0;
	}

}