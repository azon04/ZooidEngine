#ifndef __RENDERING_ZOOID__
#define __RENDERING_ZOOID__

#include "Zooid/BaseZooid.h"
#include "Utils/Macros.h"
#include "Memory/Handle.h"
#include "Structs.h"

namespace ZE
{
	class IRenderer;

	class RenderZooid : public BaseZooid
	{
	public:

		RenderZooid(GameContext* gameContext) : BaseZooid(gameContext)
		{}

		virtual Handle CreateRenderBufferData() = 0;
		virtual Handle CreateRenderBufferArray() = 0;
		virtual Handle CreateRenderTexture() = 0;
		virtual Handle CreateShader() = 0;
		virtual Handle CreateShaderChain() = 0;
		virtual Handle CreateRenderBuffer() = 0;
		virtual Handle CreateFrameBuffer() = 0;
		virtual Handle CreateBlendState(const RenderBlendState& renderBlendState) = 0;
		virtual Handle CreateDepthStencilState(const RenderDepthStencilState& renderDepthStencilState) = 0;
		virtual Handle CreateRasterizerState(const RenderRasterizerState& renderRasterizerState) = 0;

		FORCEINLINE IRenderer* GetRenderer() const { return m_renderer; }

	protected:
		IRenderer* m_renderer;
	};
}

#endif
