#ifndef __GL_RENDERING_ZOOID__
#define __GL_RENDERING_ZOOID__

#include "Renderer/RenderZooid.h"

namespace ZE
{
	class GLRenderZooid : public RenderZooid
	{
	public:
		GLRenderZooid(GameContext* gameContext) : RenderZooid(gameContext)
		{}

		// RenderZooid implementation
		virtual Handle CreateRenderBufferData() override;
		virtual Handle CreateRenderBufferArray() override;
		virtual Handle CreateRenderTexture() override;
		virtual Handle CreateShader() override;
		virtual Handle CreateShaderChain() override;
		virtual Handle CreateRenderBuffer() override;
		virtual Handle CreateFrameBuffer() override;

		virtual void Init() override;
		virtual void Destroy() override;
	};
}
#endif
