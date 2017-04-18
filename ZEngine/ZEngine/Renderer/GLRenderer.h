#ifndef __ZE_GL_RENDERER__
#define __ZE_GL_RENDERER__

#include "IRenderer.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <vector>

#define WIDTH 1024
#define HEIGHT 768

namespace ZE {
class GLRenderer : public IRenderer {
	
public:
	GLRenderer() {}
	virtual ~GLRenderer() {}

	virtual void Setup() override;
	virtual void BeginRender() override;
	virtual void EndRender() override;
	virtual void Clean() override;
	virtual void ClearScreen() override;
	virtual void Draw(ShaderAction* shaderAction) override;
	virtual bool IsClose() override;

	GLFWwindow* m_window;
};
}
#endif
