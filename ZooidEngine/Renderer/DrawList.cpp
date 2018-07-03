#include "DrawList.h"

#include "ResourceManagers/BufferManager.h"

namespace ZE 
{

	void DrawList::Reset()
	{
		for (int i = 0; i < m_size; i++)
		{
			m_drawList[i].reset();
		}

		for (int i = 0; i < m_secondPassSize; i++)
		{
			m_secondPassDrawList[i].reset();
		}

		m_size = 0;
		m_secondPassSize = 0;
	}

	void DrawList::Setup()
	{
		BufferManager* pBufferManager = BufferManager::getInstance();
		m_mainConstantBuffer = pBufferManager->createConstantBuffer(&m_shaderData, sizeof(ShaderData));
		m_lightConstantBuffer = pBufferManager->createConstantBuffer(&m_lightData, sizeof(LightData));
	}

}