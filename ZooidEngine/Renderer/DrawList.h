#ifndef __ZE_DRAW_LIST__
#define __ZE_DRAW_LIST__

#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Math/Shapes.h"
#include "ShaderData.h"
#include "SceneRenderer/RenderGatherer.h"
#include "SceneRenderer/ViewFrustum.h"
#include "SceneRenderer/RenderCommand.h"

namespace ZE 
{
	class IGPUTexture;
	class IGPUFrameBuffer;

	struct LightShadowMapData
	{
		UInt32 lightIndex;
		Int32 cascadeIndex;
		IGPUTexture* staticShadowTexture;
		IGPUTexture* dynamicShadowTexture;
		IGPUFrameBuffer* dynamicShadowFrameBuffer;

		Matrix4x4 view;
		Matrix4x4 projection;
		Matrix4x4 cullingBoxTransform;

		Frustum lightFrustum;
		bool bCull; // Whether we should cull this shadow map or no.

		// Mesh list to render at this shadow depth generation
		MeshRenderGatherer meshRenderGatherer;
		SkinMeshRenderGatherer skinMeshRenderGatherer;

		// Shadow data
		UInt32 shadowHeight;
		UInt32 shadowWidth;
	};

	struct EnvironmentMapData
	{
		IGPUTexture* irradianceMap;
		IGPUTexture* prefilterSpecularMap;

		// #TODO add type ie: skybox or a probe; add position where the probe is
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

		ShaderData m_shaderFrameData;
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

		// #TODO this will be available for debug only
		MeshRenderGatherer m_debugMeshRenderGatherer;

		// Highlight
		MeshRenderGatherer m_highligtRenderGatherer;

		// View Fustrum
		ViewFrustum m_viewFustrum;
		Matrix4x4 m_projectionMat;
		Matrix4x4 m_viewMat;

		// Objects Bounding Volume
		AxisAlignedBox m_objectsBounding;

		EnvironmentMapData& getNextEnvironmentMap()
		{
			return m_environmentMaps[m_environmentMapSize++];
		}

		// Environment map list
		EnvironmentMapData m_environmentMaps[8];

		UInt32 m_environmentMapSize;

		// Render Command function
		RenderCommandList m_commandList;

#if ZE_RENDER_MULTITHREAD
		Mutex m_mutex;
		bool m_bReady = false;
#endif
	};

};
#endif
