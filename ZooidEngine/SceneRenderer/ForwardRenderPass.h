#ifndef __ZE_FORWARD_RENDER_PASS_H__
#define __ZE_FORWARD_RENDER_PASS_H__

#include "RenderPass.h"
#include "Common/SingletonClass.h"

namespace ZE
{
	class IGPUTexture;
	class IGPUFrameBuffer;
	class IGPURenderBuffer;

	class ForwardRenderPass : public RenderPass, public Singleton<ForwardRenderPass>
	{
	public:

		ForwardRenderPass() : m_resultFrameBuffer(nullptr), m_resultTexture(nullptr), m_depthRenderBuffer(nullptr)
		{}

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "ForwardRenderPass"; }
	protected:
		IGPUTexture* m_resultTexture;
		IGPUFrameBuffer* m_resultFrameBuffer;
		IGPURenderBuffer* m_depthRenderBuffer;
	};
}
#endif
