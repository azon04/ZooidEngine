#ifndef __Z_RENDERER__
#define __Z_RENDERER__

#include "Utils/PrimitiveTypes.h"
#include "Enums.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

namespace ZE 
{
	struct ShaderData;
	class ShaderAction;
	class DrawList;
	class IShaderChain;
	class IGPUBufferArray;

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

		// Process Shadow Map
		virtual void ProcessShadowMapList(DrawList* drawList, bool bWithStatic) = 0;

		// Process Drawlist
		virtual void ProcessDrawList(DrawList* drawList) = 0;

		// Draw based on parameters in shaderAction
		virtual void Draw(DrawList* drawList, ShaderAction* shaderAction) = 0;

		// Draw GPUBufferArray Only
		virtual void DrawBufferArray(IShaderChain* shader, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset = 0) = 0;

		// Draw GPUBufferArray Instance
		virtual void DrawBufferArrayInstanced(IShaderChain* shader, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset, UInt32 instanceCount) = 0;

		// Process Shader Action
		virtual void ProcessShaderAction(DrawList* drawList, ShaderAction* shaderAction) = 0;

		// Poll Event : This is used to tell that renderer still active
		virtual void PollEvent() = 0;

		// Multithread: call this before processing renderer related asset: texture, buffer, etc 
		virtual void AcquireRenderThreadOwnership() = 0;

		// Multithread: call this after processing renderer related assets: texture, buffers, etc
		virtual void ReleaseRenderThreadOwnership() = 0;

		// Multithread: check RenderThreadOwnership
		virtual bool HasRenderThreadOwnership() = 0;

		// Enable Renderer feature. See RendererFeature
		virtual void EnableFeature(UInt32 feature) = 0;

		// Disable Renderer Feature. See RendererFeature
		virtual void DisableFeature(UInt32 feature) = 0;

		// Reset feature to default. See RendererFeature
		virtual void ResetFeature(UInt32 feature) = 0;

		// Check if feature enabled. See RendererFeature
		virtual bool IsFeatureEnabled(UInt32 feature) { return false; };
	
		// Is renderer closed
		virtual bool IsClose() { return false; };

		// Get render texture or screen height
		virtual float GetHeight() const = 0;

		// Get render texture or screen height
		virtual float GetWidth() const = 0;

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
