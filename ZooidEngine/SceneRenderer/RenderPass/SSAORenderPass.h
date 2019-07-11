#ifndef __ZE_SSAO_RENDER_PASS_H__
#define __ZE_SSAO_RENDER_PASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IShaderChain;
	class IGPUTexture;
	class IGPURenderBuffer;
	class IGPUFrameBuffer;
	class IGPUBufferData;

	class SSAORenderPass : public RenderPass, public Singleton<SSAORenderPass>
	{
	public:

		SSAORenderPass();

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "SSAORenderPass"; }

	protected:

		// Default Shader Chain
		IShaderChain* m_shaderChain;
		IShaderChain* m_blurShaderChain;

		// Texture and FrameBuffers
		Array<Vector4> m_ssaoKernels;
		IGPUBufferData* m_ssaoBufferData;
		IGPUTexture* m_noiseTexture;
		IGPUTexture* m_ssaoResultTexture;
		IGPUTexture* m_ssaoBlurResultTexture;
		IGPURenderBuffer* m_depthRenderBuffer;
		IGPUFrameBuffer* m_ssaoFrameBuffer;
		IGPUFrameBuffer* m_ssaoBlurFrameBuffer;
	};
}
#endif
