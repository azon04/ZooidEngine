#ifndef __ZE_SHADOW_RENDERER_H__
#define __ZE_SHADOW_RENDERER_H__

#include "SceneRenderer.h"
#include "Utils/Array.h"
#include "Renderer/ShaderData.h"

namespace ZE
{
	struct LightStruct;
	struct LightShadowMapData;
	class DrawList;
	class IGPUFrameBuffer;
	class IShaderChain;
	class IGPUTexture;
	class IGPUBufferData;

	class ShadowDepthRenderer : public ISceneRenderer
	{
	public:

		ShadowDepthRenderer();

		virtual void setupShadowMapData(LightShadowMapData* shadowMapData);
		virtual void reset();
		
		virtual void beginRender() override;
		virtual void render(RenderInfo* renderInfos, UInt32 renderInfoCount) override;
		virtual void endRender() override;

		static void Setup(LightShadowMapData* shadowMapData, LightStruct* lightData, DrawList* drawList);
		static void Render(RenderInfo* renderInfos, UInt32 renderInfoCount, bool bSkinMesh);
		static void BindShadowTextures(DrawList* _drawList, IShaderChain* shaderChain, UInt32 textureIndexOffset);
		static void BindShadowTexturesIndexed(DrawList* _drawList, IShaderChain* shaderChain, UInt32 textureIndexOffset, Array<Int32>& TextureIds);
		static void BeginRender() { getInstance()->beginRender(); }
		static void EndRender() { getInstance()->endRender(); }
		static void Reset() { getInstance()->reset(); }

		static ShadowDepthRenderer* getInstance();

	protected:

		IGPUFrameBuffer* m_currentFrameBuffer;
		UInt32 m_shadowHeight;
		UInt32 m_shadowWidth;
		IShaderChain* m_shaderChain;
		IShaderChain* m_skinnedShaderChain;
		bool m_bRenderSkinMesh;
		Array<IGPUTexture*> m_shadowTextures;

		LightStruct* m_currentLight;

		ShaderData m_shaderData;
		IGPUBufferData* m_shaderConstantBuffer;

	private:
		static ShadowDepthRenderer* s_shadowRenderer;
	};

}
#endif
