#ifndef __ZE_GL_RENDERER__
#define __ZE_GL_RENDERER__

#include "Renderer/IRenderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Platform/Thread.h"
#include "Utils/HashMap.h"

#define WIDTH 1280
#define HEIGHT 720

namespace ZE 
{

class BufferData;
class IGPUBufferData;

class GLRenderer : public IRenderer 
{
	
public:
	GLRenderer();
	virtual ~GLRenderer() {}

	// IRenderer implementation
	virtual void Setup() override;
	virtual void BeginRender() override;
	virtual void EndRender() override;
	virtual void Clean() override;
	virtual void ClearScreen() override;
	virtual void Clear(UInt32 clearBits) override;
	virtual void DrawBufferArray(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset = 0) override;
	virtual void DrawBufferArrayInstanced(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset, UInt32 instanceCount) override;
	virtual void DrawIndexed(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count, void* indices) override;
	virtual void DrawIndexedInstanced(ERenderTopologyEnum primitiveTopology, UInt32 indexStride, UInt32 count, void* indices, UInt32 instanceCount) override;
	virtual void DrawArray(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count) override;
	virtual void DrawInstanced(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count, UInt32 instanceCount) override;
	virtual void DrawTextureToScreen(IGPUTexture* texture, const Vector2& screenPos, const Vector2& screenDimension) override;

	virtual void SetRenderBlendState(const RenderBlendState& renderBlendState) override;
	virtual void SetRenderDepthStencilState(const RenderDepthStencilState& renderDepthStencilState) override;
	virtual void SetRenderRasterizerState(const RenderRasterizerState& renderRasterizerState) override;

	virtual bool IsClose() override;

	virtual void PollEvent() override;

	virtual void AcquireRenderThreadOwnership() override;
	virtual void ReleaseRenderThreadOwnership() override;
	virtual bool HasRenderThreadOwnership() override;

	virtual float GetHeight() const { return (float)m_height; }
	virtual float GetWidth() const { return (float)m_width; }

	virtual void Resize(UInt32 width, UInt32 height);

#if defined(_WIN32) || defined(_WIN64)
	virtual HWND getWinWindow() override
	{
		return m_winWindow;
	}

private:
	HWND m_winWindow;
#endif

private:
	
	GLFWwindow* m_window;
	GLFWwindow* m_renderWindow;
	Mutex m_renderLock;
	ThreadId m_currentThreadHasLock;

	// Window Screen Dimension
	Int32 m_width;
	Int32 m_height;
};
}
#endif
