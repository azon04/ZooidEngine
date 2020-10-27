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
			m_shaderChain(nullptr),
			m_bCulled(false)
		{
			
		}

		IShader* m_vertexShader;
		IShader* m_pixelShader;
		IShaderChain* m_shaderChain;
		IGPUBufferArray* m_gpuBufferArray;
		Matrix4x4 m_worldTransform;
		Vector3 m_boxExtent;
		UInt32 drawCount;
		Vector3 m_boxLocalPos;
		ERenderTopologyEnum m_renderTopology;
		bool m_bCulled;
	};
};

#endif
