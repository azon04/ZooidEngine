#ifndef __Z_RENDERER__
#define __Z_RENDERER__

namespace ZE {

struct ShaderData;
class ShaderAction;
class DrawList;

class IRenderer {
public:
	IRenderer() {}
	virtual ~IRenderer() {}

	virtual void Setup() = 0;
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual void Clean() = 0;
	virtual void ClearScreen() = 0;
	virtual void ProcessDrawList(DrawList* drawList) = 0;
	virtual void Draw(ShaderAction* shaderAction) = 0;
	virtual void ProcessShaderAction(ShaderAction* shaderAction) = 0;
	virtual bool IsClose() { return false; };

	virtual float GetHeight() const = 0;
	virtual float GetWidth() const = 0;
};

}
#endif
