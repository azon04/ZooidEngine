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
		m_mainConstantBuffer = pBufferManager->createConstantBuffer(&m_shaderData, sizeof(ShaderData));
		m_lightConstantBuffer = pBufferManager->createConstantBuffer(&m_lightData, sizeof(LightData));
	}

}