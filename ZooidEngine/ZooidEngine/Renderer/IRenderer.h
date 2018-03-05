#ifndef __Z_RENDERER__
#define __Z_RENDERER__

#include "Utils/PrimitiveTypes.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

namespace ZE {

struct ShaderData;
class ShaderAction;
class DrawList;

enum RendererFeature : UInt32
{
	DEPTH_TEST = 0,
	STENCIL_TEST = 1
};

enum RendererCompareFunc : UInt32
{
	NEVER,
	LESS,
	LEQUAL,
	GREATER,
	GEQUAL,
	EQUAL,
	NOTEQUAL,
	ALWAYS
};

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

	virtual void PollEvent() = 0;

	virtual void AcquireRenderThreadOwnership() = 0;
	virtual void ReleaseRenderThreadOwnership() = 0;

	virtual void EnableFeature(UInt32 feature) = 0;
	virtual void DisableFeature(UInt32 feature) = 0;
	virtual void ResetFeature(UInt32 feature) = 0;

	virtual bool IsFeatureEnabled(UInt32 feature) { return false; };
	
	virtual bool IsClose() { return false; };

	virtual float GetHeight() const = 0;
	virtual float GetWidth() const = 0;

#if defined(_WIN32) || defined(_WIN64)
	virtual HWND getWinWindow() = 0;
#endif

};

}
#endif
