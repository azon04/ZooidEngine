#ifndef __Z_RENDERER__
#define __Z_RENDERER__



namespace ZE {

class ShaderAction;

class IRenderer {
public:
	IRenderer() {}
	virtual ~IRenderer() {}

	virtual void Setup() = 0;
	virtual void BeginRender() = 0;
	virtual void EndRender() = 0;
	virtual void Clean() = 0;
	virtual void ClearScreen() = 0;
	virtual void Draw(ShaderAction* shaderAction) = 0;
	virtual void ProcessShaderAction(ShaderAction* shaderAction) = 0;
	virtual bool IsClose() { return false; };
};

}
#endif
