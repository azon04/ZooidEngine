#ifndef __Z_RENDERER__
#define __Z_RENDERER__

#include "Utils/PrimitiveTypes.h"

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#endif

namespace ZE 
{

struct ShaderData;
class ShaderAction;
class DrawList;

enum RendererFeature : UInt32
{
	DEPTH_TEST = 0,
	STENCIL_TEST = 1,
	BLEND = 2,
	FACE_CULING = 3
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

enum RendererBlendFactor : UInt32
{
	ZERO,
	ONE,
	SRC_COLOR,
	ONE_MINUS_SRC_COLOR,
	DST_COLOR,
	ONE_MINUS_DST_COLOR,
	SRC_ALPHA,
	ONE_MINUS_SRC_ALPHA,
	DST_ALPHA,
	ONE_MINUS_DST_ALPHA,
	CONSTANT_COLOR,
	ONE_MINUS_CONSTANT_COLOR,
	CONSTANT_ALPHA,
	ONE_MINUS_CONSTANT_ALPHA
};

enum CullFace : UInt8
{
	FRONT,
	BACK,
	FRONT_AND_BACK
};

enum FaceFrontOrder : UInt8
{
	CCW,
	CW
};

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

	// Process Drawlist
	virtual void ProcessDrawList(DrawList* drawList) = 0;

	// Draw based on parameters in shaderAction
	virtual void Draw(ShaderAction* shaderAction) = 0;

	// Process Shader Action
	virtual void ProcessShaderAction(ShaderAction* shaderAction) = 0;

	// Poll Event : This is used to tell that renderer still active
	virtual void PollEvent() = 0;

	// Multithread: call this before processing renderer related asset: texture, buffer, etc 
	virtual void AcquireRenderThreadOwnership() = 0;

	// Multithread: call this after processing renderer related assets: texture, buffers, etc
	virtual void ReleaseRenderThreadOwnership() = 0;

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

}
#endif
