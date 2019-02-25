#ifndef __ZE_SKYBOX_RENDERINFO_H__
#define __ZE_SKYBOX_RENDERINFO_H__

#include "MeshRenderInfo.h"

namespace ZE
{
	class SkyboxRenderInfo : public RenderInfo
	{
	public:
		SkyboxRenderInfo() :
			m_cubeTexture(nullptr)
		{}

		class IGPUTexture* m_cubeTexture;
	};
}
#endif
