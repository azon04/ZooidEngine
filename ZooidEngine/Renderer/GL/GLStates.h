#ifndef __ZE_GL_STATES_H__
#define __ZE_GL_STATES_H__

#include "Renderer/IGPUStates.h"

#include "GL/glew.h"

namespace ZE
{
	class GLDepthStencilState : public IGPUDepthStencilState
	{
	public:
		virtual void setup(const RenderDepthStencilState& state) override;

		bool m_depthEnabled;
		bool m_stencilEnabled;

		bool m_enableDepthMask;

		GLenum m_depthTestFunc;
		GLenum m_stencilTestFunc;

		UInt32 m_stencilRefMask;
		UInt32 m_stencilMask;
		UInt32 m_stencilWriteMask;

		GLenum m_stencilFailOp;
		GLenum m_depthFailOp;
		GLenum m_depthStencilPassOp;
	};

	class GLRasterizerState : public IGPURasterizerState
	{
	public:
		virtual void setup(const RenderRasterizerState& state) override;

		GLenum m_front;
		GLenum m_cullMode;
		GLenum m_fillMode;
	};

	class GLBlendState : public IGPUBlendState
	{
	public:
		virtual void setup(const RenderBlendState& state) override;

		bool m_blendEnabled;

		GLenum m_sourceBlendFactor;
		GLenum m_destBlendFactor;

		UInt32 m_alphaRef;
	};
}

#endif
