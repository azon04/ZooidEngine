#ifndef __ZE_GPU_STATES_H__
#define __ZE_GPU_STATES_H__

#include "Structs.h"
#include "ZEGameContext.h"
#include "RenderZooid.h"
#include "Utils/Array.h"

namespace ZE
{
	class IGPUState
	{
	public:
		~IGPUState() { release(); }
		virtual void release() { }

		static void ClearGPUStates();

		static void AddGPUState(IGPUState* state);

	protected:
		static Array<IGPUState*> s_gpuStateCollections;
	};

	#define CREATE_GPU_STATE_CLASS(GPUClassName, StateStructName) \
		class GPUClassName : public IGPUState \
		{  \
		public: \
			virtual void setup(const StateStructName& state) = 0; \
		};

	CREATE_GPU_STATE_CLASS(IGPUBlendState, RenderBlendState)
	CREATE_GPU_STATE_CLASS(IGPUDepthStencilState, RenderDepthStencilState)
	CREATE_GPU_STATE_CLASS(IGPURasterizerState, RenderRasterizerState)
	
	template<EFaceFrontOrder frontFace, ECullFace cullMode, ERenderFillMode fillMode>
	class TRenderRasterizerState
	{
	public:
		static RenderRasterizerState& GetStatic()
		{
			static RenderRasterizerState rasterizerState
			{
				frontFace,
				cullMode,
				fillMode
			};

			return rasterizerState;
		}

		static IGPURasterizerState* GetGPUState()
		{
			if (!s_gpuState)
			{
				s_gpuState = gGameContext->getRenderZooid()->CreateRasterizerState(GetStatic()).getObject<IGPURasterizerState>();
				IGPUState::AddGPUState(s_gpuState);
			}

			return s_gpuState;
		};

	private:
		static IGPURasterizerState* s_gpuState;
	};

	template<EFaceFrontOrder frontFace, ECullFace cullMode, ERenderFillMode fillMode>
	IGPURasterizerState* ZE::TRenderRasterizerState<frontFace, cullMode, fillMode>::s_gpuState = nullptr;

	template<bool depthEnabled,
		bool stencilEnabled,
		bool enableDepthMask,
		ERendererCompareFunc depthTestFunc,
		ERendererCompareFunc stencilTestFunc,
		UInt32 stencilRefMask,
		UInt32 stencilMask,
		UInt32 stencilWriteMask,
		ERenderDepthStencilOps stencilFail = DS_OP_KEEP,
		ERenderDepthStencilOps depthFail = DS_OP_KEEP,
		ERenderDepthStencilOps depthStencilPass = DS_OP_REPLACE>
		class TRenderDepthStencilState
	{
	public:
		static RenderDepthStencilState& GetStatic()
		{
			static RenderDepthStencilState depthStencilState
			{
				depthEnabled,
				stencilEnabled,
				enableDepthMask,
				depthTestFunc,
				stencilTestFunc,
				stencilRefMask,
				stencilMask,
				stencilWriteMask,
				stencilFail,
				depthFail,
				depthStencilPass
			};

			return depthStencilState;
		}

		static IGPUDepthStencilState* GetGPUState()
		{
			if (!s_gpuState)
			{
				s_gpuState = gGameContext->getRenderZooid()->CreateDepthStencilState(GetStatic()).getObject<IGPUDepthStencilState>();
				IGPUState::AddGPUState(s_gpuState);
			}

			return s_gpuState;
		};

	private:
		static IGPUDepthStencilState* s_gpuState;
	};

	template<bool depthEnabled,
		bool stencilEnabled,
		bool enableDepthMask,
		ERendererCompareFunc depthTestFunc,
		ERendererCompareFunc stencilTestFunc,
		UInt32 stencilRefMask,
		UInt32 stencilMask,
		UInt32 stencilWriteMask,
		ERenderDepthStencilOps stencilFail /*= DS_OP_KEEP*/,
		ERenderDepthStencilOps depthFail /*= DS_OP_KEEP*/,
		ERenderDepthStencilOps depthStencilPass /*= DS_OP_REPLACE*/>
	IGPUDepthStencilState* ZE::TRenderDepthStencilState<depthEnabled, stencilEnabled, enableDepthMask, depthTestFunc, stencilTestFunc, stencilRefMask, stencilMask, stencilWriteMask, stencilFail, depthFail, depthStencilPass>::s_gpuState = nullptr;

	template<bool blendEnabled, 
		ERendererBlendFactor sourceBlendFactor = ERendererBlendFactor::ONE, 
		ERendererBlendFactor destBlendFactor = ERendererBlendFactor::ONE, 
		ERendererBlendMode blendMode = ERendererBlendMode::BLEND_MODE_ADD 
	>
	class TRenderBlendState
	{
	public:
		static RenderBlendState& GetStatic()
		{
			static RenderBlendState blendState
			{
				blendEnabled,
				sourceBlendFactor,
				destBlendFactor,
				blendMode
			};

			return blendState;
		}

		static IGPUBlendState* GetGPUState()
		{
			if (!s_gpuState)
			{
				s_gpuState = gGameContext->getRenderZooid()->CreateBlendState(GetStatic()).getObject<IGPUBlendState>();
				IGPUState::AddGPUState(s_gpuState);
			}

			return s_gpuState;
		};

	private:
		static IGPUBlendState* s_gpuState;

	};

	template<bool blendEnabled, ERendererBlendFactor sourceBlendFactor /*= ERendererBlendFactor::ONE*/, ERendererBlendFactor destBlendFactor /*= ERendererBlendFactor::ONE*/, ERendererBlendMode blendMode>
	IGPUBlendState* ZE::TRenderBlendState<blendEnabled, sourceBlendFactor, destBlendFactor, blendMode>::s_gpuState = nullptr;

	// Default States
	typedef TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::BACK, ERenderFillMode::MODE_FILL> DefaultRasterizerState;
	typedef TRenderDepthStencilState<true, false, true, ERendererCompareFunc::LESS, ERendererCompareFunc::ALWAYS, 1, 0x00, 0xFF> DefaultDepthStencilState;
	typedef TRenderBlendState<false> DefaultRenderBlendState;
};

#endif // !__ZE_GPU_STATES_H__
