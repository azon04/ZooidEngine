#include "Skybox.h"

#include "Resources/Texture.h"
#include "ResourceManagers/TextureManager.h"
#include "ResourceManagers/BufferManager.h"
#include "Renderer/BufferData.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IRenderer.h"
#include "Renderer/DrawList.h"

#include "SceneRenderer/Utils/EquiRectangularToCubeMap.h"
#include "SceneRenderer/Utils/EMToIrradianceMap.h"

#include "ResourceManagers/ShaderManager.h"

#include "Events/Events.h"
#include "ZEGameContext.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(Skybox, SceneComponent);

	void Skybox::loadFromPath(const char* filePath)
	{
		m_cubeMapTexture = TextureManager::GetInstance()->loadResource<IGPUTexture>(filePath);
		ZASSERT(m_cubeMapTexture, "Failed to load cube map");

		if (!m_cubeMapTexture->isCubeTexture())
		{
			Handle resultHandle = EquiRectangularToCubeMap::ConvertToCubeMap(gGameContext, m_cubeMapTexture);
			ZCHECK(resultHandle.isValid());
			m_cubeMapTexture = resultHandle.getObject<IGPUTexture>();
			TextureManager::GetInstance()->unloadResource(filePath);
		}

		Handle irradianceMapHandle = EMToIrradianceMap::ConvertToIrradianceMap(gGameContext, m_cubeMapTexture);
		m_irradianceMap = irradianceMapHandle.getObject<IGPUTexture>();
	}

	void Skybox::setupComponent()
	{
		SceneComponent::setupComponent();

		addEventDelegate(Event_GATHER_RENDER, &Skybox::handleGatherRender);

		if (!s_skyBoxBufferArray)
		{
			BufferData bufferData(VERTEX_BUFFER);

			Float32 skybox_vertices[] =
			{
				// positions          
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,

				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,

				-1.0f,  1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,

				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f,  1.0f
			};

			ScopedRenderThreadOwnership renderOwnership(gGameContext->getRenderer());

			bufferData.SetData(skybox_vertices, sizeof(Float32) * 3, 36);
			bufferData.setBufferLayout(BUFFER_LAYOUT_V3);
			bufferData.setStaticBuffer(true);

			s_skyBoxBufferArray = BufferManager::getInstance()->createBufferArray(&bufferData, nullptr, nullptr).getObject<IGPUBufferArray>();
		}
	}

	void Skybox::handleGatherRender(Event* pEvent)
	{
		SkyboxRenderInfo* renderInfo = gGameContext->getDrawList()->m_skyboxRenderGatherer.nextRenderInfo();

		renderInfo->m_renderTopology = TOPOLOGY_TRIANGLE;
		renderInfo->m_gpuBufferArray = s_skyBoxBufferArray;
		renderInfo->drawCount = 36;
		renderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_SKYBOX);
		renderInfo->m_cubeTexture = m_cubeMapTexture;
		renderInfo->m_irradianceMap = m_irradianceMap;

		if (m_irradianceMap)
		{
			EnvironmentMapData& envMap = gGameContext->getDrawList()->getNextEnvironmentMap();
			envMap.environmentMapCube = m_irradianceMap;
		}
	}

	ZE::IGPUBufferArray* Skybox::s_skyBoxBufferArray = nullptr;

};