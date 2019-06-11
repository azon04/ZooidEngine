#ifndef __ZE_RENDERER_ENUMS_H__
#define __ZE_RENDERER_ENUMS_H__

#include "Utils/PrimitiveTypes.h"

namespace ZE
{
	enum EBufferType
	{
		UNIFORM_BUFFER,
		VERTEX_BUFFER,
		INDEX_BUFFER,
		COMPUTE_BUFFER
	};

	enum EDataType
	{
		FLOAT = 1,
		INTEGER = 2,
		UNSIGNED_INTEGER = 3,
		BYTE = 4,
		UNSIGNED_BYTE = 5
	};

	enum EFrameBuferAttachmentType
	{
		NONE_ATTACHMENT = 0,
		COLOR_ATTACHMENT = 1,
		DEPTH_ATTACHMENT = 2,
		STENCIL_ATTACHMENT = 3,
		DEPTH_STENCIL_ATTACHMENT = 4
	};

	enum ERenderBufferFormat
	{
		COLOR_RGB = 0,
		COLOR_RGB_ALPHA = 1,
		COLOR_sRGB = 2,
		COLOR_sRGB_ALPHA = 3,
		DEPTH_ONLY = 4,
		DEPTH_STENCIL = 5,
		STENCIL_ONLY = 6
	};

	enum RendererFeature : UInt32
	{
		DEPTH_TEST = 0,
		STENCIL_TEST = 1,
		BLEND = 2,
		FACE_CULING = 3
	};

	enum ERendererCompareFunc : UInt32
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

	enum ERendererBlendFactor : UInt32
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

	enum ECullFace : UInt8
	{
		FRONT,
		BACK,
		FRONT_AND_BACK,
		CULL_NONE
	};

	enum EFaceFrontOrder : UInt8
	{
		CCW,
		CW
	};

	enum EShaderType : UInt8
	{
		SHADER_VERTEX = 0,
		SHADER_PIXEL = 1,
		SHADER_GEOMETRY = 2,
		SHADER_COMPUTE = 3,
		SHADER_MAX
	};

	enum ERenderTopologyEnum : UInt8
	{
		TOPOLOGY_TRIANGLE,
		TOPOLOGY_POINT,
		TOPOLOGY_LINE,
		TOPOLOGY_LINE_STRIP
	};

	enum ETextureWrap : UInt8
	{
		CLAMP_TO_EDGE,
		MIRRORED_REPEAT,
		REPEAT,
		CLAMP_TO_BORDER
	};

	enum ETextureFilter : UInt8
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};

	enum ETextureFormat : UInt8
	{
		TEX_NONE = 0,
		TEX_RED = 1,
		TEX_RED_GREEN = 2,
		TEX_RGB = 3,
		TEX_RGBA = 4,
		TEX_DEPTH = 5,
		TEX_DEPTH_STENCIL = 6,
		TEX_sRGB,
		TEX_sRGBA,
		TEX_R8,
		TEX_R16,
		TEX_R16F,
		TEX_RG8,
		TEX_RG16,
		TEX_RGB16F,
		TEX_RGBA16F,
		TEX_MAX
	};

	enum ERenderBufferBit : UInt32
	{
		COLOR_BUFFER_BIT = 1 << 1,
		DEPTH_BUFFER_BIT = 1 << 2,
		STENCIL_BUFFER_BIT = 1 << 3
	};

	enum ERenderFillMode
	{
		MODE_POINT,
		MODE_LINE,
		MODE_FILL
	};

	enum ERenderDepthStencilOps : UInt32
	{
		DS_OP_KEEP,
		DS_OP_ZERO,
		DS_OP_REPLACE,
		DS_OP_INCR,
		DS_OP_INCR_WRAP,
		DS_OP_DECR,
		DS_OP_DECR_WRAP,
		DS_OP_INVERT
	};
}
#endif // !__ZE_RENDERER_ENUMS_H__

