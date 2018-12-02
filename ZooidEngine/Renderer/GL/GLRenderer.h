#ifndef __ZE_GL_RENDERER__
#define __ZE_GL_RENDERER__

#include "Renderer/IRenderer.h"
#include "Renderer/ShaderAction.h"

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
	virtual void ProcessShadowMapList(DrawList* drawList, bool bWithStatic) override;
	virtual void ProcessDrawList(DrawList* drawList) override;
	virtual void ProcessShaderAction(DrawList* drawList, ShaderAction* shaderAction) override;
	virtual void Draw(DrawList* drawList, ShaderAction* shaderAction) override; 
	virtual void DrawBufferArray(IShaderChain* shader, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset = 0) override;
	virtual void DrawBufferArrayInstanced(IShaderChain* shader, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset, UInt32 instanceCount) override;
	virtual void DrawTextureToScreen(IGPUTexture* texture, const Vector2& screenPos, const Vector2& screenDimension) override;

	virtual bool IsClose() override;

	virtual void PollEvent() override;

	virtual void AcquireRenderThreadOwnership() override;
	virtual void ReleaseRenderThreadOwnership() override;
	virtual bool HasRenderThreadOwnership() override;

	virtual void EnableFeature(UInt32 feature) override;
	virtual void DisableFeature(UInt32 feature) override;
	virtual bool IsFeatureEnabled(UInt32 feature) override;
	virtual void ResetFeature(UInt32 feature) override;
	virtual void ProcessShaderFeature(ShaderFeature& shaderFeature);

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

	void DrawEx(DrawList* drawList, ShaderAction* shaderAction, bool bWithShadow);
	void BindShadowMaps(DrawList* _drawList, IShaderChain* shaderChain, int offset);
	void UnbindShadowMaps(DrawList* _drawList, IShaderChain* shaderChain, int offset);
	void SortBlendShaderActions(const Vector3& cameraPosition, const Vector3& cameraDirection,  ShaderAction* inArray, UInt32* outIndexArray, UInt32 count);

	GLFWwindow* m_window;
	GLFWwindow* m_renderWindow;
	Mutex m_renderLock;
	HashMap<UInt32, GLenum> HashFeatureToRealGLVar;
	HashMap<UInt32, GLenum> HashCompareFuncToRealGLVar;
	HashMap<UInt32, GLenum> HashBlendFactorToRealGLVar;
	GLenum HashFaceFrontOrder[2];
	GLenum HashCullFace[3];
	ThreadId m_currentThreadHasLock;

	// Some Base Resource buffer
	GLuint m_quadVBO;
	GLuint m_quadVAO;

	// Window Screen Dimension
	Int32 m_width;
	Int32 m_height;
};
}
#endif
