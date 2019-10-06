#include "DrawList.h"

#include "ResourceManagers/BufferManager.h"

namespace ZE 
{

	void DrawList::Reset()
	{
		m_lightShadowSize = 0;

		m_meshRenderGatherer.reset();
		m_skinMeshRenderGatherer.reset();
		m_transculentRenderGatherer.reset();
		m_skyboxRenderGatherer.reset();
		m_textSceneRenderGatherer.reset();
		m_textScreenRenderGatherer.reset();

		m_debugMeshRenderGatherer.reset();
	}

	void DrawList::Setup()
	{
		BufferManager* pBufferManager = BufferManager::getInstance();
		m_mainConstantBuffer = pBufferManager->createConstantBuffer(&m_shaderFrameData, sizeof(ShaderData), CONSTANT_BUFFER_SHADER_DATA_INDEX);
		m_lightConstantBuffer = pBufferManager->createConstantBuffer(&m_lightData, sizeof(LightData), CONSTANT_BUFFER_LIGHT_DATA_INDEX);
		Reset();
	}

}