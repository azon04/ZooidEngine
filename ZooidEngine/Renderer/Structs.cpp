#include "Structs.h"

namespace ZE
{
	TextureFormatStruct TextureFormats[ETextureFormat::TEX_MAX] =
	{
		{ ETextureFormat::TEX_NONE,				ETextureFormat::TEX_NONE },
		{ ETextureFormat::TEX_RED,				ETextureFormat::TEX_RED },
		{ ETextureFormat::TEX_RED_GREEN,		ETextureFormat::TEX_RED_GREEN },
		{ ETextureFormat::TEX_RGB,				ETextureFormat::TEX_RGB },
		{ ETextureFormat::TEX_RGBA,				ETextureFormat::TEX_RGBA },
		{ ETextureFormat::TEX_DEPTH,			ETextureFormat::TEX_DEPTH },
		{ ETextureFormat::TEX_DEPTH_STENCIL,	ETextureFormat::TEX_DEPTH_STENCIL },
		{ ETextureFormat::TEX_DEPTH24_STENCIL8,	ETextureFormat::TEX_DEPTH_STENCIL },
		{ ETextureFormat::TEX_sRGB,				ETextureFormat::TEX_RGB },
		{ ETextureFormat::TEX_sRGBA,			ETextureFormat::TEX_RGBA },
		{ ETextureFormat::TEX_R8,				ETextureFormat::TEX_RED },
		{ ETextureFormat::TEX_R16,				ETextureFormat::TEX_RED },
		{ ETextureFormat::TEX_R16F,				ETextureFormat::TEX_RED },
		{ ETextureFormat::TEX_RG8,				ETextureFormat::TEX_RED_GREEN },
		{ ETextureFormat::TEX_RGBA8,			ETextureFormat::TEX_RGBA },
		{ ETextureFormat::TEX_RG16,				ETextureFormat::TEX_RED_GREEN },
		{ ETextureFormat::TEX_RG16F,			ETextureFormat::TEX_RED_GREEN },
		{ ETextureFormat::TEX_RGB16F,			ETextureFormat::TEX_RGB },
		{ ETextureFormat::TEX_RGBA16F,			ETextureFormat::TEX_RGBA },
		{ ETextureFormat::TEX_RGBA32F,			ETextureFormat::TEX_RGB },
		{ ETextureFormat::TEX_RGBA32F,			ETextureFormat::TEX_RGBA }

	};
}