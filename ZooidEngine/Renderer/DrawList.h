#ifndef __ZE_DRAW_LIST__
#define __ZE_DRAW_LIST__

#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"
#include "ShaderAction.h"
#include "ShaderData.h"

#define MAX_STATIC_SHADOW_LIST 512
#define MAX_DYNAMIC_SHADOW_LIST 512
#define MAX_DRAW_LIST 1024
#define MAX_SECONDPASS_DRAW_LIST 1024

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
			m_size = 0;
			m_secondPassSize = 0;
			m_staticShadowObjSize = 0;
			m_dynamicShadowObjSize = 0;
			m_lightShadowSize = 0;
			m_shadowMapSize = 0;
		}

		void Reset();
		void Setup();

		ShaderAction& getNextShaderAction() 
		{
			return m_drawList[m_size++];
		}

		ShaderAction& getNextSecondPassShaderAction()
		{
			return m_secondPassDrawList[m_secondPassSize++];
		}

		ShaderAction& getNextStaticShadowShaderAction()
		{
			return m_staticShadowObjList[m_staticShadowObjSize++];
		}

		ShaderAction& getNextDynamicShadowShaderAction()
		{
			return m_dynamicShadowObjList[m_dynamicShadowObjSize++];
		}

		LightShadowMapData& getNextLightShadowMapData()
		{
			return m_lightShadowMapData[m_lightShadowSize++];
		}

		ShaderAction m_dynamicShadowObjList[MAX_DRAW_LIST];
		ShaderAction m_staticShadowObjList[MAX_DRAW_LIST];
		ShaderAction m_drawList[MAX_DRAW_LIST];
		ShaderAction m_secondPassDrawList[MAX_SECONDPASS_DRAW_LIST];
		LightShadowMapData m_lightShadowMapData[MAX_NUMBER_OF_LIGHT];

		UInt32 m_dynamicShadowObjSize;
		UInt32 m_staticShadowObjSize;
		UInt32 m_size;
		UInt32 m_secondPassSize;
		UInt32 m_lightShadowSize;

		IGPUTexture* m_shadowMap[MAX_NUMBER_OF_LIGHT * 2];
		UInt32 m_shadowMapSize;

		ShaderData m_shaderData;
		IGPUBufferData* m_mainConstantBuffer;

		LightData m_lightData;
		IGPUBufferData* m_lightConstantBuffer;

		Vector3 m_cameraPosition;
		Vector3 m_cameraDirection;
	};

};
#endif
