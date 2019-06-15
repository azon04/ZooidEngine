#ifndef __ZE_LIGHT_RENDER_PASS_H__
#define __ZE_LIGHT_RENDER_PASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IShaderChain;
	class IGPUTexture;
	class IGPUFrameBuffer;
	class IGPURenderBuffer;

	class LightRenderPass : public RenderPass, public Singleton<LightRenderPass>
	{
	public:

		LightRenderPass();

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "LightRenderPasss"; }

	protected:
		// Default Shader Chain
		IShaderChain* m_shaderChain;

		// Textures and FrameBuffers
		IGPUTexture* m_resultPassTexture;
		IGPURenderBuffer* m_depthRenderBuffer;
		IGPUFrameBuffer* m_resultFrameBuffer;
	};
}
#endif
