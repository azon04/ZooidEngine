#ifndef __ZE_TEXT_RENDERER_H__
#define __ZE_TEXT_RENDERER_H__

#include "SceneRenderer.h"

namespace ZE
{
	class TextSceneRenderer : public ISceneRenderer, public Singleton<TextSceneRenderer>
	{
	public:

		virtual void beginRender() override;
		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) override;
		virtual void endRender() override;

		static void Render(RenderInfo* renderInfos, UInt32 renderInfoCount);
	};

	class TextScreenRenderer : public ISceneRenderer, public Singleton<TextScreenRenderer>
	{
	public:

		virtual void beginRender() override;
		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) override;
		virtual void endRender() override;

		static void Render(RenderInfo* renderInfos, UInt32 renderInfoCount);
	};
};
#endif
