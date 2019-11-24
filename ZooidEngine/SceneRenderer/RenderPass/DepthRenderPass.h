#ifndef __ZE_DEPTH_RENDER_PASS_H__
#define __ZE_DEPTH_RENDER_PASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IShaderChain;
	class IGPUTexture;
	class IGPURenderBuffer;

	class DepthRenderPass : public RenderPass, public Singleton<DepthRenderPass>
	{
	public:

		DepthRenderPass();

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "DepthRenderPass"; }

		void doOcclusionSceneQueries(RenderInfo* renderInfos, UInt32 renderInfoCount, bool bUsingSkeleton = false);

	protected:

		// Default Shader Chain
		IShaderChain* m_shaderChain;
		IShaderChain* m_skinnedShaderChain;

		// Texture and FrameBuffer
		IGPUTexture* m_depthTexture;
		IGPUFrameBuffer* m_frameBuffer;
	};
}
#endif
