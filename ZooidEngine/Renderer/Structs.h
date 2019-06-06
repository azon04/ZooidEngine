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

		bool EnableDepthMask;
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

	struct TextureFormatStruct
	{
		ETextureFormat TextureFormat;
		ETextureFormat BaseFormat;
	};

	extern TextureFormatStruct TextureFormats[ETextureFormat::TEX_MAX];

}

#endif