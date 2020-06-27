#ifndef __ZE_GL_RENDERER__
#define __ZE_GL_RENDERER__

#include "Renderer/IRenderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Platform/Thread.h"
#include "Utils/HashMap.h"

#define ZE_GL_VERSION_MAJOR 4
#define ZE_GL_VERSION_MINOR 3
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
	virtual void BeginFrame() override;
	virtual void EndFrame() override;
	virtual void Clean() override;
	virtual void ClearScreen() override;
	virtual void Clear(UInt32 clearBits) override;
	virtual void ResetViewport() override;
	virtual void SetViewport(Int32 x, Int32 y, Int32 width, Int32 height) override;
	virtual void DrawBufferArray(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset = 0) override;
	virtual void DrawBufferArrayInstanced(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset, UInt32 instanceCount) override;
	virtual void DrawIndexed(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count, void* indices) override;
	virtual void DrawIndexedInstanced(ERenderTopologyEnum primitiveTopology, UInt32 indexStride, UInt32 count, void* indices, UInt32 instanceCount) override;
	virtual void DrawArray(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count) override;
	virtual void DrawInstanced(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count, UInt32 instanceCount) override;
	virtual void DrawTextureToScreen(IGPUTexture* texture, const Vector2& screenPos, const Vector2& screenDimension) override;
	virtual void CopyFrameBuffer(IGPUFrameBuffer* frameBufferFrom, IGPUFrameBuffer* frameBufferTo, Int32 srcX0, Int32 srcY0, Int32 srcX1, Int32 srcY1, Int32 dstX0, Int32 dstY0, Int32 dstX1, Int32 dstY1, UInt32 mask, ETextureFilter filter) override;

	virtual void PushDebugGroup(const char* groupName) override;
	virtual void PopDebugGroup() override;

	virtual void SetRenderBlendState(IGPUBlendState* renderBlendState) override;
	virtual void SetBlendColorRef(Float32 red, Float32 green, Float32 blue, Float32 alpha) override;
	virtual void SetRenderDepthStencilState(IGPUDepthStencilState* renderDepthStencilState) override;
	virtual void SetRenderRasterizerState(IGPURasterizerState* renderRasterizerState) override;
	virtual void MaskColor(bool redMask, bool greenMask, bool blueMask, bool alphaMask) override;

	virtual bool IsClose() override;

	virtual void PollEvent() override;

	virtual void AcquireRenderThreadOwnership() override;
	virtual void ReleaseRenderThreadOwnership() override;
	virtual bool HasRenderThreadOwnership() override;

	virtual float GetHeight() const { return (float)m_height; }
	virtual float GetWidth() const { return (float)m_width; }

	virtual void Resize(UInt32 width, UInt32 height);

	// Flush all commands
	virtual void FlushCommands();

	// This will stall the CPU until all commands executed
	virtual void FinishCommands();

	virtual UInt32 BeginQuery(ERenderQueryType type) override;
	virtual bool CheckQueryAvailable(UInt32 queryId) override;
	virtual void RetreiveQueryResult(ERenderQueryType type, UInt32 queryId, UInt32* result) override;
	virtual void EndQuery(ERenderQueryType type, UInt32 queryId) override;

	FORCEINLINE virtual void* getWindowContext()
	{
		return m_window;
	}

private:
	
	GLFWwindow* m_window;
	GLFWwindow* m_renderWindow;
	Mutex m_renderLock;
	ThreadId m_currentThreadHasLock;

	// Window Screen Dimension
	Int32 m_width;
	Int32 m_height;

	UInt32 currentDebugId;
	UInt32 currentQueryId;

	Array<GLuint> m_queryIds;
};
}
#endif
