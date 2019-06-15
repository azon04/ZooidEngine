#ifndef __ZE_SCREEN_RENDER_PASS_H__
#define __ZE_SCREEN_RENDER_PASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IShaderChain;

	class ScreenRenderPass : public RenderPass, public Singleton<ScreenRenderPass>
	{
	public:
		ScreenRenderPass();

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "ScreenRenderPass"; }

	private:

		IShaderChain* m_shaderChain;
	};
}
#endif // !__ZE_SCREEN_RENDER_PASS_H__
