#ifndef __Z_RENDERER__
#define __Z_RENDERER__

#include "Math/Vector2.h"
#include "Utils/PrimitiveTypes.h"
#include "Enums.h"
#include "Structs.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

namespace ZE 
{
	struct ShaderData;
	class DrawList;
	class IShaderChain;
	class IGPUBufferArray;
	class IGPUTexture;
	class IGPUDepthStencilState;
	class IGPUBlendState;
	class IGPURasterizerState;
	class IGPUFrameBuffer;

	class IRenderer {
	public:
		IRenderer() {}
		virtual ~IRenderer() {}

		// Setup renderer
		virtual void Setup() = 0;
	
		// Begin render
		virtual void BeginRender() = 0;

		// End render
		virtual void EndRender() = 0;

		// Clean renderer
		virtual void Clean() = 0;

		// Clear Screen
		virtual void ClearScreen() = 0;

		// Clear buffer bit
		// if FrameBuffer is bound, then clear bit for that FrameBuffer, otherwise it will clear bits in Screen
		virtual void Clear(UInt32 clearBits) = 0;

		// Draw GPUBufferArray Only
		virtual void DrawBufferArray(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset = 0) = 0;

		// Draw GPUBufferArray Instance
		virtual void DrawBufferArrayInstanced(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset, UInt32 instanceCount) = 0;

		// Draw Indexed
		virtual void DrawIndexed(ERenderTopologyEnum primitiveTopology, UInt32 indexStride, UInt32 count, void* indices) = 0;

		// Draw Indexed Instance
		virtual void DrawIndexedInstanced(ERenderTopologyEnum primitiveTopology, UInt32 indexStride, UInt32 count, void* indices, UInt32 instanceCount) = 0;

		// Draw Array
		virtual void DrawArray(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count) = 0;

		// Draw Array Instanced
		virtual void DrawInstanced(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count, UInt32 instanceCount) = 0;

		// Utility Function to draw texture to screen
		virtual void DrawTextureToScreen(IGPUTexture* texture, const Vector2& screenPos, const Vector2& screenDimension) = 0;

		// Set Blend State
		virtual void SetRenderBlendState(IGPUBlendState* renderBlendState) = 0;

		// Set Depth Stencil State
		virtual void SetRenderDepthStencilState(IGPUDepthStencilState* renderDepthStencilState) = 0;

		// Set Render Rasterizer State
		virtual void SetRenderRasterizerState(IGPURasterizerState* renderRasterizerState) = 0;

		// Copy Frame Buffer to another Frame Buffer
		virtual void CopyFrameBuffer(IGPUFrameBuffer* frameBufferFrom, IGPUFrameBuffer* frameBufferTo, Int32 srcX0, Int32 srcY0, Int32 srcX1, Int32 srcY1, Int32 dstX0, Int32 dstY0, Int32 dstX1, Int32 dstY1, UInt32 mask, ETextureFilter filter) = 0;

		// Poll Event : This is used to tell that renderer still active
		virtual void PollEvent() = 0;

		// Multithread: call this before processing renderer related asset: texture, buffer, etc 
		virtual void AcquireRenderThreadOwnership() = 0;

		// Multithread: call this after processing renderer related assets: texture, buffers, etc
		virtual void ReleaseRenderThreadOwnership() = 0;

		// Multithread: check RenderThreadOwnership
		virtual bool HasRenderThreadOwnership() = 0;
	
		// Is renderer closed
		virtual bool IsClose() { return false; };

		// Get render texture or screen height
		virtual float GetHeight() const = 0;

		// Get render texture or screen height
		virtual float GetWidth() const = 0;

		// Reset viewport
		virtual void ResetViewport() = 0;

		// Resize Window
		virtual void Resize(UInt32 width, UInt32 height) = 0;

	#if defined(_WIN32) || defined(_WIN64)
		virtual HWND getWinWindow() = 0;
	#endif

	};

	// Helper to scope getting and releasing RenderThread lock
	class ScopedRenderThreadOwnership
	{
	public:
		ScopedRenderThreadOwnership(IRenderer* renderer)
			: m_renderer(renderer)
		{
			m_hasLock = m_renderer->HasRenderThreadOwnership();
			if (!m_hasLock)
			{
				m_renderer->AcquireRenderThreadOwnership();
			}
		}

		~ScopedRenderThreadOwnership()
		{
			if (!m_hasLock)
			{
				m_renderer->ReleaseRenderThreadOwnership();
			}
		}

	protected:
		bool m_hasLock;
		IRenderer* m_renderer;
	};
}
#endif
