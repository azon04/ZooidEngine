#ifndef __RENDERING_ZOOID__
#define __RENDERING_ZOOID__

#include "Zooid/BaseZooid.h"
#include "Utils/Macros.h"
#include "Memory/Handle.h"

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

		FORCEINLINE IRenderer* GetRenderer() const { return m_renderer; }

		IRenderer* m_renderer;
	};
}

#endif
