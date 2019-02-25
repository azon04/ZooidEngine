#ifndef __ZE_DRAW_LIST__
#define __ZE_DRAW_LIST__

#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"
#include "ShaderData.h"
#include "SceneRenderer/RenderGatherer.h"

namespace ZE 
{
	class IGPUTexture;
	class IGPUFrameBuffer;

	struct LightShadowMapData
	{
		UInt32 lightIndex;
		IGPUTexture* staticShadowTexture;
		IGPUTexture* dynamicShadowTexture;
		IGPUFrameBuffer* dynamicShadowFrameBuffer;
		IShaderChain* normalShaderChain;
		IShaderChain* skinnedShaderChain;
	};

	class DrawList 
	{

	public:
		
		DrawList() 
		{
			m_lightShadowSize = 0;
		}

		void Reset();
		void Setup();

		LightShadowMapData& getNextLightShadowMapData()
		{
			return m_lightShadowMapData[m_lightShadowSize++];
		}

		LightShadowMapData m_lightShadowMapData[MAX_NUMBER_OF_LIGHT];

		UInt32 m_lightShadowSize;

		ShaderData m_shaderData;
		IGPUBufferData* m_mainConstantBuffer;

		LightData m_lightData;
		IGPUBufferData* m_lightConstantBuffer;

		Vector3 m_cameraPosition;
		Vector3 m_cameraDirection;

		// List Render Infos
		MeshRenderGatherer m_meshRenderGatherer;
		SkinMeshRenderGatherer m_skinMeshRenderGatherer;
		TransculentRenderGatherer m_transculentRenderGatherer;
		SkyboxRenderGatherer m_skyboxRenderGatherer;
		TextRenderGatherer m_textSceneRenderGatherer;
		TextRenderGatherer m_textScreenRenderGatherer;
	};

};
#endif
