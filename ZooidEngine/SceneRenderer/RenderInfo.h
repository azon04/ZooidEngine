#ifndef __ZE_RENDER_INFO_H__
#define __ZE_RENDER_INFO_H__

#include "Renderer/Enums.h"
#include "Math/Matrix4x4.h"

namespace ZE
{
	class IShader;
	class IShaderChain;
	class IGPUBufferArray;

	class RenderInfo
	{
	public:
		RenderInfo() :
			m_vertexShader(nullptr),
			m_pixelShader(nullptr),
			m_shaderChain(nullptr)
		{
			
		}

		IShader* m_vertexShader;
		IShader* m_pixelShader;
		IShaderChain* m_shaderChain;
		IGPUBufferArray* m_gpuBufferArray;
		ERenderTopologyEnum m_renderTopology;
		UInt32 drawCount;
		Matrix4x4 m_worldTransform;
	};
};

#endif
