#include "DrawList.h"

#include "ResourceManagers/BufferManager.h"

namespace ZE 
{

	void DrawList::Reset()
	{

		for (UInt32 i = 0; i < m_staticShadowObjSize; i++)
		{
			m_staticShadowObjList[i].reset();
		}

		for (UInt32 i = 0; i < m_dynamicShadowObjSize; i++)
		{
			m_dynamicShadowObjList[i].reset();
		}

		for (UInt32 i = 0; i < m_size; i++)
		{
			m_drawList[i].reset();
		}

		for (UInt32 i = 0; i < m_secondPassSize; i++)
		{
			m_secondPassDrawList[i].reset();
		}

		m_staticShadowObjSize = 0;
		m_dynamicShadowObjSize = 0;
		m_size = 0;
		m_secondPassSize = 0;
		m_lightShadowSize = 0;
		m_shadowMapSize = 0;
	}

	void DrawList::Setup()
	{
		BufferManager* pBufferManager = BufferManager::getInstance();
		m_mainConstantBuffer = pBufferManager->createConstantBuffer(&m_shaderData, sizeof(ShaderData));
		m_lightConstantBuffer = pBufferManager->createConstantBuffer(&m_lightData, sizeof(LightData));
	}

}