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
	virtual void ProcessShadowMapList(DrawList* drawList, bool bWithStatic) override;
	virtual void ProcessDrawList(DrawList* drawList) override;
	virtual void ProcessShaderAction(DrawList* drawList, ShaderAction* shaderAction) override;
	virtual void Draw(DrawList* drawList, ShaderAction* shaderAction) override;
	virtual bool IsClose() override;

	virtual void PollEvent() override;

	virtual void AcquireRenderThreadOwnership() override;
	virtual void ReleaseRenderThreadOwnership() override;

	virtual void EnableFeature(UInt32 feature) override;
	virtual void DisableFeature(UInt32 feature) override;
	virtual bool IsFeatureEnabled(UInt32 feature) override;
	virtual void ResetFeature(UInt32 feature) override;

	virtual float GetHeight() const { return HEIGHT; }
	virtual float GetWidth() const { return WIDTH; }

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
	void ProcessShaderFeature(ShaderFeature& shaderFeature);
	void SortBlendShaderActions(const Vector3& cameraPosition, const Vector3& cameraDirection,  ShaderAction* inArray, UInt32* outIndexArray, UInt32 count);

	GLFWwindow* m_window;
	GLFWwindow* m_renderWindow;
	Mutex m_renderLock;
	HashMap<UInt32, GLenum> HashFeatureToRealGLVar;
	HashMap<UInt32, GLenum> HashCompareFuncToRealGLVar;
	HashMap<UInt32, GLenum> HashBlendFactorToRealGLVar;
	GLenum HashFaceFrontOrder[2];
	GLenum HashCullFace[3];
};
}
#endif
