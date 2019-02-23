#ifndef __ZE_RENDERER_STRUCTS_H__
#define __ZE_RENDERER_STRUCTS_H__

#include "Utils/PrimitiveTypes.h"
#include "Enums.h"

namespace ZE
{
	struct RenderBlendState
	{
		bool BlendEnabled;

		ERendererBlendFactor SourceBlendFactor;
		ERendererBlendFactor DestBlendFactor;

		UInt32 AlphaRef;
	};

	struct RenderDepthStencilState
	{
		bool DepthEnabled;
		bool StencilEnabled;

		ERendererCompareFunc DepthTestFunc;
		ERendererCompareFunc StencilTestFunc;

		UInt32 StencilRefMask;
		UInt32 StencilMask;
		UInt32 StencilWriteMask;

		ERenderDepthStencilOps StencilFailOp;
		ERenderDepthStencilOps DepthFailOp;
		ERenderDepthStencilOps DepthStencilPassOp;

	};

	struct RenderRasterizerState
	{
		EFaceFrontOrder Front;
		ECullFace CullMode;
		ERenderFillMode FillMode;
	};

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
	};


	template<bool depthEnabled, 
			bool stencilEnabled, 
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
	};

	template<bool blendEnabled, ERendererBlendFactor sourceBlendFactor = ERendererBlendFactor::ONE, ERendererBlendFactor destBlendFactor = ERendererBlendFactor::ONE, UInt32 AlphaRef = 0 >
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
				AlphaRef
			};

			return blendState;
		}
	};

	// Default States
	typedef TRenderRasterizerState<EFaceFrontOrder::CCW, ECullFace::BACK, ERenderFillMode::MODE_FILL> DefaultRasterizerState;
	typedef TRenderDepthStencilState<true, false, ERendererCompareFunc::LESS, ERendererCompareFunc::ALWAYS, 1, 0x00, 0xFF> DefaultDepthStencilState;
	typedef TRenderBlendState<false> DefaultRenderBlendState;


}

#endif