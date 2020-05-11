#ifndef __ZE_LIGHT_RENDER_PASS_H__
#define __ZE_LIGHT_RENDER_PASS_H__

#include "SceneRenderer/RenderPass.h"
#include "Common/SingletonClass.h"
#include "Renderer/ShaderData.h"

// #TODO Make this can be setting using configuration file in the future
#define RENDER_LIGHT_PASS_NAIVE 0
#define RENDER_LIGHT_PASS_LIGHT_INDEXED 1 // #TODO WIP Light Indexed Pass
#define RENDER_LIGHT_PASS_PER_TYPE 2

#define RENDER_LIGHT_PASS_ALG RENDER_LIGHT_PASS_PER_TYPE

namespace ZE
{
	class IShaderChain;
	class IGPUTexture;
	class IGPUFrameBuffer;
	class IGPURenderBuffer;
	class IGPUBufferArray;
	class IRenderer;

#if RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_PER_TYPE
	struct LightSampleData
	{
		Vector3 viewPos;
		Float32 padding;

		LightStruct light;
		CascadeShadowData cascadeShadowData[4];
		ShadowData shadowData[8];
	};
#endif

	class LightRenderPass : public RenderPass, public Singleton<LightRenderPass>
	{
	public:

		LightRenderPass();

		virtual void prepare(GameContext* _gameContext) override;
		virtual void release(GameContext* _gameContext) override;

		virtual void begin(GameContext* _gameContext) override;
		virtual void end(GameContext* _gameContext) override;

		virtual bool execute_CPU(GameContext* _gameContext) override;
		virtual bool execute_GPU(GameContext* _gameContext) override;

		const char* getRenderPassName() const { return "LightRenderPasss"; }

	private:
#if RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_LIGHT_INDEXED
		void drawLightVolume(Int32 lightIndex, IRenderer* renderer, GameContext* gameContex);
#endif
		void createEnvBRDFLUT();

	protected:
		// Default Shader Chain
		IShaderChain* m_shaderChain;

		// Textures and FrameBuffers
		IGPUTexture* m_resultPassTexture;
		IGPUTexture* m_depthTexture;
		IGPUFrameBuffer* m_resultFrameBuffer;

		// Light Volume Shapes
		IGPUBufferArray* m_sphereBufferArray;
		IGPUBufferArray* m_coneBufferArray;

		// BRDF Spec LUT
		IGPUTexture* m_envBRDFLUT = nullptr;

#if RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_LIGHT_INDEXED
		IShaderChain* m_lightIndexedVolumeShader;

		IGPUTexture* m_lightVolumeIndexTexture;
		IGPUFrameBuffer* m_lightVolumeFrameBuffer;
#elif RENDER_LIGHT_PASS_ALG == RENDER_LIGHT_PASS_PER_TYPE
		
		// Shader for Ambient calculation
		IShaderChain* m_ambientShader;
		IShaderChain* m_ambientShaderNoIrradianceMap;

		// Shader Per Light
		IShaderChain* m_directionalShader;
		IShaderChain* m_spotLightShader;
		IShaderChain* m_pointLightShader;

		LightSampleData m_lightSampleData;
		IGPUBufferData* m_lightSampleGPUBuffer;
#endif
	};
}
#endif
