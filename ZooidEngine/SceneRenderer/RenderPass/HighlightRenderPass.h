#ifndef __ZE_HIGHLIGHT_RENDERPASS_H__
#define __ZE_HIGHLIGHT_RENDERPASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"
#include "Math/Vector3.h"

#define HIGHLIGHT_NAIVE 0
#define HIGHLIGHT_SHELL 1
#define HIGHLIGHT_IMAGE_PROCESS 2 // #TODO Implement

#define HIGHLIGHT_ALG HIGHLIGHT_SHELL

namespace ZE
{
	class IShaderChain;

	class HighlightRenderPass : public RenderPass, public Singleton<HighlightRenderPass>
	{
	public:

		HighlightRenderPass() : m_highLightColor(0.0f, 1.0f, 0.0f) {}

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "HighlightPass"; }

	protected:
		Vector3 m_highLightColor;

#if HIGHLIGHT_ALG == HIGHLIGHT_SHELL
		IShaderChain* m_shader;
#endif
	};
}
#endif
