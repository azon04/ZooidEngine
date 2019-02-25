#ifndef __ZE_SKYBOX_RENDERER_H__
#define __ZE_SKYBOX_RENDERER_H__

#include "SceneRenderer.h"

namespace ZE
{
	class SkyBoxRenderer : public ISceneRenderer, public Singleton<SkyBoxRenderer>
	{
	public:
		virtual void beginRender() override;
		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) override;
		virtual void endRender() override;

		static void Render(RenderInfo* renderInfos, UInt32 renderInfoCount);
	};
}
#endif
