#ifndef __ZE_GBUFFER_RENDER_PASS_H__
#define __ZE_GBUFFER_RENDER_PASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IShaderChain;
	class IGPUTexture;
	class IGPURenderBuffer;
	class IGPUFrameBuffer;

	class GBufferRenderPass : public RenderPass, public Singleton<GBufferRenderPass>
	{
	public:

		GBufferRenderPass();
		
		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "GBufferRenderPass"; }

	protected:
		// Default Shader Chain
		IShaderChain* m_shaderChain;
		IShaderChain* m_shaderSkinChain;

		// Textures and FrameBuffers
		IGPUTexture* m_positionTexture;
		IGPUTexture* m_normalTexture;
		IGPUTexture* m_albedoTexture;
		IGPUTexture* m_ambientTexture;
		IGPUTexture* m_specTexture;
		IGPUTexture* m_renderDepthTexture;
		IGPUFrameBuffer* m_frameBuffer;

	};
}
#endif
