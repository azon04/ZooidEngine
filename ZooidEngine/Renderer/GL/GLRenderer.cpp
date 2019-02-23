#include "GLRenderer.h"

#include "ZEngine.h"
#include "Enums.h"
#include "Platform/Platform.h"

#include "Renderer/IGPUTexture.h"

#include "ResourceManagers/ShaderManager.h"

#if defined(_WIN32) || defined(_WIN64)
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "GLFW/glfw3native.h"
#include <cstring>

namespace ZE 
{
	GLRenderer* s_renderer;

	GLRenderer::GLRenderer()
	{
		m_width = WIDTH;
		m_height = HEIGHT;

		s_renderer = this;
	}

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

	GLenum getRealFeature(RendererFeature feature)
	{
		switch (feature)
		{
		case RendererFeature::DEPTH_TEST:
			return GL_DEPTH_TEST;
		case RendererFeature::STENCIL_TEST:
			return GL_STENCIL_TEST;
		case RendererFeature::BLEND:
			return GL_BLEND;
		case RendererFeature::FACE_CULING:
			return GL_CULL_FACE;
		default:
			ZASSERT(false, "Feature not found!");
			return 0;
		}
	}

	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		s_renderer->Resize((UInt32)width, (UInt32)height);
	}

	void GLRenderer::Setup()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		
		m_window = glfwCreateWindow(WIDTH, HEIGHT, "ZooidEngine - Windows", nullptr, nullptr);
		if (!m_window) 
		{
			glfwTerminate();
			ZASSERT(true, "Failed to create GLFW window");
			return;
		}
		glfwMakeContextCurrent(m_window);

#if defined(_WIN32) || defined(_WIN64)
		// Setup WinWindow
		m_winWindow = glfwGetWin32Window(m_window);
#endif

		// Init glfw
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (err != GLEW_OK) 
		{
			glfwTerminate();
			ZASSERT(false, "GL Init Error: %s", glewGetErrorString(err));
			return;
		}
		
		// Viewport setup
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		// Callback when window Frame Buffer resize
		glfwSetFramebufferSizeCallback(m_window, FrameBufferSizeCallback);

		// Enable Depth test
		SetRenderDepthStencilState(DefaultDepthStencilState::GetStatic());		

		// Enable Face Culling
		SetRenderRasterizerState(DefaultRasterizerState::GetStatic());
	}

	void GLRenderer::BeginRender()
	{
		glViewport(0, 0, m_width, m_height);
	}

	void GLRenderer::EndRender()
	{
		// Swap buffer
		glfwSwapBuffers(m_window);
	}

	void GLRenderer::Clean()
	{
		glfwTerminate();
	}

	void GLRenderer::ClearScreen()
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glStencilMask(0xFF); // Before cleaning Stencil buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00); // Disable writing to stencil buffer
	}

	void GLRenderer::Clear(UInt32 clearBits)
	{
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(getClearBitMask(clearBits));
	}

	GLenum translateTopology(ERenderTopologyEnum topology)
	{
		switch (topology)
		{
		case TOPOLOGY_LINE:			return GL_LINES;
		case TOPOLOGY_TRIANGLE:		return GL_TRIANGLES;
		case TOPOLOGY_POINT:		return GL_POINTS;
		default:					return GL_TRIANGLES;
		}
	}

	void GLRenderer::DrawBufferArray(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset)
	{
		gpuBufferArray->bind();
		if (gpuBufferArray->isUsingIndexBuffer())
		{
			DrawIndexed(primitiveTopology, gpuBufferArray->getIndexBuffer()->getStride(), count, 0);
		}
		else
		{
			DrawArray(primitiveTopology, offset, count);
		}
		gpuBufferArray->unbind();
	}

	void GLRenderer::DrawBufferArrayInstanced(ERenderTopologyEnum primitiveTopology, IGPUBufferArray* gpuBufferArray, UInt32 count, UInt32 offset, UInt32 instanceCount)
	{
		gpuBufferArray->bind();
		if (gpuBufferArray->isUsingIndexBuffer())
		{
			DrawIndexedInstanced(primitiveTopology, gpuBufferArray->getIndexBuffer()->getStride(), count, 0, instanceCount);
		}
		else
		{
			DrawInstanced(primitiveTopology, offset, count, instanceCount);
		}
		gpuBufferArray->unbind();
	}

	void GLRenderer::DrawIndexed(ERenderTopologyEnum primitiveTopology, UInt32 indexStride, UInt32 count, void* indices)
	{
		GLenum drawTopology = translateTopology(primitiveTopology);

		glDrawElements(drawTopology, count, indexStride == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, indices);
	}

	void GLRenderer::DrawIndexedInstanced(ERenderTopologyEnum primitiveTopology, UInt32 indexStride, UInt32 count, void* indices, UInt32 instanceCount)
	{
		GLenum drawTopology = translateTopology(primitiveTopology);

		glDrawElementsInstanced(drawTopology, count, indexStride == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, indices, instanceCount);
	}

	void GLRenderer::DrawArray(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count)
	{
		GLenum drawTopology = translateTopology(primitiveTopology);

		glDrawArrays(drawTopology, offset, count);
	}

	void GLRenderer::DrawInstanced(ERenderTopologyEnum primitiveTopology, UInt32 offset, UInt32 count, UInt32 instanceCount)
	{
		GLenum drawTopology = translateTopology(primitiveTopology);

		glDrawArraysInstanced(drawTopology, offset, count, instanceCount);
	}

	void GLRenderer::DrawTextureToScreen(IGPUTexture* texture, const Vector2& screenPos, const Vector2& screenDimension)
	{
		IShaderChain* shader = ShaderManager::GetInstance()->getShaderChain(Z_SHADER_CHAIN_DRAW_SCREEN_QUAD);
		IGPUBufferArray* quadVAO = BufferManager::getInstance()->getBufferArray(BUFFER_ARRAY_QUAD_V3_TC2);

		shader->bind();
		shader->setVec2("offsetPos", screenPos);
		shader->setVec2("dimension", screenDimension);
		shader->setTexture("InTexture", texture, 0);
		texture->bind();

		DrawBufferArray(TOPOLOGY_TRIANGLE, quadVAO, 6);

		texture->unbind();
		shader->unbind();
	}

	void GLRenderer::SetRenderBlendState(const RenderBlendState& renderBlendState)
	{
		if (renderBlendState.BlendEnabled)
		{
			glEnable(GL_BLEND);
		}
		else
		{
			glDisable(GL_BLEND);
		}


		if (renderBlendState.BlendEnabled)
		{
			glBlendFunc(getBlendFactor(renderBlendState.SourceBlendFactor), getBlendFactor(renderBlendState.DestBlendFactor));
		}
	}

	void GLRenderer::SetRenderDepthStencilState(const RenderDepthStencilState& renderDepthStencilState)
	{
		if (renderDepthStencilState.DepthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		glDepthFunc(getCompareFunc(renderDepthStencilState.DepthTestFunc));

		if (renderDepthStencilState.StencilEnabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		glStencilFunc(getCompareFunc(renderDepthStencilState.StencilTestFunc), renderDepthStencilState.StencilRefMask, renderDepthStencilState.StencilMask);
		glStencilMask(renderDepthStencilState.StencilWriteMask);

		glStencilOp(getStencilOp(renderDepthStencilState.StencilFailOp), 
			getStencilOp(renderDepthStencilState.DepthFailOp), 
			getStencilOp(renderDepthStencilState.DepthStencilPassOp));
	}

	void GLRenderer::SetRenderRasterizerState(const RenderRasterizerState& renderRasterizerState)
	{
		glFrontFace(getFaceFrontOrder(renderRasterizerState.Front));

		if (renderRasterizerState.CullMode == CULL_NONE)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(getCullFace(renderRasterizerState.CullMode));
		}
	}

	bool GLRenderer::IsClose()
	{
		return glfwWindowShouldClose(m_window) == 1;
	}

	void GLRenderer::PollEvent()
	{
		glfwPollEvents();
		if ( glfwWindowShouldClose(m_window) == 1 )
		{
			Platform::RequestExit(0);
		}
	}

	void GLRenderer::AcquireRenderThreadOwnership()
	{
		m_renderLock.lock();
		glfwMakeContextCurrent(m_window);
		m_currentThreadHasLock = ZE::getThreadId();
	}

	void GLRenderer::ReleaseRenderThreadOwnership()
	{	
		glfwMakeContextCurrent(nullptr);
		m_renderLock.unlock();
		m_currentThreadHasLock = ThreadId();
	}

	bool GLRenderer::HasRenderThreadOwnership()
	{
		return m_currentThreadHasLock == ZE::getThreadId();
	}

	void GLRenderer::Resize(UInt32 width, UInt32 height)
	{
		m_width = (Int32)width;
		m_height = (Int32)height;
	}

}