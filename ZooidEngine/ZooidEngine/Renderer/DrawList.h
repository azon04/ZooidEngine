#ifndef __ZE_DRAW_LIST__
#define __ZE_DRAW_LIST__

#include "Utils/PrimitiveTypes.h"
#include "Math/Vector3.h"
#include "ShaderAction.h"
#include "ShaderData.h"

#define MAX_DRAW_LIST 1024
#define MAX_SECONDPASS_DRAW_LIST 1024

namespace ZE 
{
	class DrawList 
	{
	public:
		
		DrawList() {
			m_size = 0;
			m_secondPassSize = 0;
		}

		void Reset();
		void Setup();

		ShaderAction& getNextShaderAction() {
			return m_drawList[m_size++];
		}

		ShaderAction& getNextSecondPassShaderAction()
		{
			return m_secondPassDrawList[m_secondPassSize++];
		}

		ShaderAction m_drawList[MAX_DRAW_LIST];
		ShaderAction m_secondPassDrawList[MAX_SECONDPASS_DRAW_LIST];

		ZE::Int32 m_size;
		ZE::Int32 m_secondPassSize;

		ShaderData m_shaderData;
		IGPUBufferData* m_mainConstantBuffer;

		LightData m_lightData;
		IGPUBufferData* m_lightConstantBuffer;

		Vector3 m_cameraPosition;
		Vector3 m_cameraDirection;
	};

};
#endif
