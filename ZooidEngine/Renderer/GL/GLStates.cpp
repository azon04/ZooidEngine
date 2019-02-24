#include "GLStates.h"

namespace ZE
{
	
	GLenum getCullFace(ECullFace cullFace)
	{
		switch (cullFace)
		{
		case ZE::FRONT:
			return GL_FRONT;
		case ZE::BACK:
			return GL_BACK;
		case ZE::FRONT_AND_BACK:
			return GL_FRONT_AND_BACK;
		case ZE::CULL_NONE:
			return 0;
		default:
			return 0;
		}
	}

	GLenum getFaceFrontOrder(EFaceFrontOrder faceOrder)
	{
		switch (faceOrder)
		{
		case ZE::CCW:
			return GL_CCW;
		case ZE::CW:
			return GL_CW;
		default:
			ZASSERT(false, "Face Front Order not found!");
			return GL_CCW;
		}
	}

	GLenum getCompareFunc(ERendererCompareFunc compareFunc)
	{
		switch (compareFunc)
		{
		case ZE::NEVER:
			return GL_NEVER;
		case ZE::LESS:
			return GL_LESS;
		case ZE::LEQUAL:
			return GL_EQUAL;
		case ZE::GREATER:
			return GL_GREATER;
		case ZE::GEQUAL:
			return GL_EQUAL;
		case ZE::EQUAL:
			return GL_EQUAL;
		case ZE::NOTEQUAL:
			return GL_NOTEQUAL;
		case ZE::ALWAYS:
			return GL_ALWAYS;
		default:
			ZASSERT(false, "Compare function not found!");
			return GL_ALWAYS;
		}
	}

	GLenum getStencilOp(ERenderDepthStencilOps stencilOp)
	{
		switch (stencilOp)
		{
		case ZE::DS_OP_KEEP:
			return GL_KEEP;
		case ZE::DS_OP_ZERO:
			return GL_ZERO;
		case ZE::DS_OP_REPLACE:
			return GL_REPLACE;
		case ZE::DS_OP_INCR:
			return GL_INCR;
		case ZE::DS_OP_INCR_WRAP:
			return GL_INCR_WRAP;
		case ZE::DS_OP_DECR:
			return GL_DECR;
		case ZE::DS_OP_DECR_WRAP:
			return GL_DECR_WRAP;
		case ZE::DS_OP_INVERT:
			return GL_INVERT;
		default:
			ZASSERT(false, "Depth Stencil Ops not found");
			return GL_KEEP;
		}
	}

	GLenum getBlendFactor(ERendererBlendFactor blendFactor)
	{
		switch (blendFactor)
		{
		case ZE::ZERO:
			return GL_ZERO;
		case ZE::ONE:
			return GL_ONE;
		case ZE::SRC_COLOR:
			return GL_SRC_COLOR;
		case ZE::ONE_MINUS_SRC_COLOR:
			return GL_ONE_MINUS_SRC_COLOR;
		case ZE::DST_COLOR:
			return GL_DST_COLOR;
		case ZE::ONE_MINUS_DST_COLOR:
			return GL_ONE_MINUS_DST_COLOR;
		case ZE::SRC_ALPHA:
			return GL_SRC_ALPHA;
		case ZE::ONE_MINUS_SRC_ALPHA:
			return GL_ONE_MINUS_SRC_ALPHA;
		case ZE::DST_ALPHA:
			return GL_DST_ALPHA;
		case ZE::ONE_MINUS_DST_ALPHA:
			return GL_ONE_MINUS_DST_ALPHA;
		case ZE::CONSTANT_COLOR:
			return GL_CONSTANT_COLOR;
		case ZE::ONE_MINUS_CONSTANT_COLOR:
			return GL_ONE_MINUS_CONSTANT_COLOR;
		case ZE::CONSTANT_ALPHA:
			return GL_CONSTANT_ALPHA;
		case ZE::ONE_MINUS_CONSTANT_ALPHA:
			return GL_ONE_MINUS_CONSTANT_ALPHA;
		default:
			ZASSERT(false, "Blend Factor not found!");
			return 0;
		}
	}
	
	GLenum getFillMode(ERenderFillMode fillMode)
	{
		switch (fillMode)
		{
		case ZE::MODE_POINT:
			return GL_POINT;
		case ZE::MODE_LINE:
			return GL_LINE;
		case ZE::MODE_FILL:
			return GL_FILL;
		default:
			ZASSERT(false, "FillMode not found!");
			return 0;
		}
	}

	void GLDepthStencilState::setup(const RenderDepthStencilState& state)
	{
		m_depthEnabled = state.DepthEnabled;
		m_stencilEnabled = state.StencilEnabled;
		
		m_depthTestFunc = getCompareFunc(state.DepthTestFunc);
		m_stencilTestFunc = getCompareFunc(state.StencilTestFunc);
		
		m_stencilMask = state.StencilMask;
		m_stencilRefMask = state.StencilRefMask;
		m_stencilWriteMask = state.StencilWriteMask;

		m_stencilFailOp = getStencilOp(state.StencilFailOp);
		m_depthFailOp = getStencilOp(state.DepthFailOp);
		m_depthStencilPassOp = getStencilOp(state.DepthStencilPassOp);
	}

	void GLRasterizerState::setup(const RenderRasterizerState& state)
	{
		m_front = getFaceFrontOrder(state.Front);
		m_cullMode = getCullFace(state.CullMode);
		m_fillMode = getFillMode(state.FillMode);
	}
	
	void GLBlendState::setup(const RenderBlendState& state)
	{
		m_blendEnabled = state.BlendEnabled;
		m_sourceBlendFactor = getBlendFactor(state.SourceBlendFactor);
		m_destBlendFactor = getBlendFactor(state.DestBlendFactor);
		m_alphaRef = state.AlphaRef;
	}
}



