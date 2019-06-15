#ifndef __ZE_SHADOW_DEPTH_PASS_H__
#define __ZE_SHADOW_DEPTH_PASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class ShadowDepthRenderPass : public RenderPass, public Singleton<ShadowDepthRenderPass>
	{
	public:
		ShadowDepthRenderPass();

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "ShadowDepthPass"; }
	};
}
#endif
