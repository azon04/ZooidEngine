#ifndef __ZE_DRAW_LIST__
#define __ZE_DRAW_LIST__

#include "../Utils/PrimitiveTypes.h"
#include "ShaderAction.h"
#include "ShaderData.h"

#define MAX_DRAW_LIST 1024

namespace ZE {
	class DrawList {
	public:
		
		DrawList() {
			m_size = 0;
		}

		void Reset();
		void Setup();

		ShaderAction& getNextShaderAction() {
			return m_drawList[m_size++];
		}

		ShaderAction m_drawList[MAX_DRAW_LIST];
		ZE::Int32 m_size;

		ShaderData m_shaderData;
		GPUBufferData* m_mainConstantBuffer;

		LightData m_lightData;
		GPUBufferData* m_lightConstantBuffer;
	};

};
#endif // 
