#ifndef __ZE_GL_RENDERER__
#define __ZE_GL_RENDERER__

#include "Renderer/IRenderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Platform/Thread.h"

#define WIDTH 1280
#define HEIGHT 720

namespace ZE {

class BufferData;
class IGPUBufferData;

class GLRenderer : public IRenderer {
	
public:
	GLRenderer() {}
	virtual ~GLRenderer() {}

	virtual void Setup() override;
	virtual void BeginRender() override;
	virtual void EndRender() override;
	virtual void Clean() override;
	virtual void ClearScreen() override;
	virtual void ProcessDrawList(DrawList* drawList) override;
	virtual void ProcessShaderAction(ShaderAction* shaderAction) override;
	virtual void Draw(ShaderAction* shaderAction) override;
	virtual bool IsClose() override;

	virtual void PollEvent() override;

	virtual void AcquireRenderThreadOwnership() override;
	virtual void ReleaseRenderThreadOwnership() override;

	virtual float GetHeight() const { return HEIGHT; }
	virtual float GetWidth() const { return WIDTH; }

	GLFWwindow* m_window;
	GLFWwindow* m_renderWindow;
	Mutex m_renderLock;
};
}
#endif
