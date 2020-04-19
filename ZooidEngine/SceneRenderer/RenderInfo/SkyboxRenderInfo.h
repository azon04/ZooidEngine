#ifndef __ZE_SKYBOX_RENDERINFO_H__
#define __ZE_SKYBOX_RENDERINFO_H__

#include "MeshRenderInfo.h"

namespace ZE
{
	class IGPUTexture;
	class SkyboxRenderInfo : public RenderInfo
	{
	public:
		SkyboxRenderInfo() :
			m_cubeTexture(nullptr),
			m_irradianceMap(nullptr)
		{}

		IGPUTexture* m_cubeTexture;
		IGPUTexture* m_irradianceMap;
		
	};
}
#endif
