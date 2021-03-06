#include "GLRenderer.h"

#include "ZEngine.h"
#include "Enums.h"
#include "Platform/Platform.h"

#include "Renderer/IGPUTexture.h"

#include "GLStates.h"

#include "GLFrameBuffer.h"

#include "ResourceManagers/ShaderManager.h"

#include "GLFW/glfw3native.h"
#include <cstring>

namespace ZE 
{
	GLRenderer* s_renderer;

	GLRenderer::GLRenderer()
	{
		m_width = gRenderWidth;
		m_height = gRenderHeight;

		s_renderer = this;
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
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, ZE_GL_VERSION_MAJOR);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, ZE_GL_VERSION_MINOR);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		
		m_window = glfwCreateWindow(m_width, m_height, "ZooidEngine - Windows", nullptr, nullptr);
		if (!m_window) 
		{
			glfwTerminate();
			ZASSERT(true, "Failed to create GLFW window");
			return;
		}
		glfwMakeContextCurrent(m_window);

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
		SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());		

		// Enable Face Culling
		SetRenderRasterizerState(DefaultRasterizerState::GetGPUState());

		// Enable Texture Cube seamless
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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

	void GLRenderer::BeginFrame()
	{
		currentDebugId = 0;
		currentQueryId = 0;
	}

	void GLRenderer::EndFrame()
	{

	}

	void GLRenderer::Clean()
	{
		if (m_queryIds.size() > 0)
		{
			glDeleteQueries(m_queryIds.size(), m_queryIds.data());
			m_queryIds.clear();
		}
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
		glClear(getRenderBitMask(clearBits));
	}

	void GLRenderer::ResetViewport()
	{
		glViewport(0, 0, m_width, m_height);
	}

	void GLRenderer::SetViewport(Int32 x, Int32 y, Int32 width, Int32 height)
	{
		glViewport(x, y, width, height);
	}

	GLenum translateTopology(ERenderTopologyEnum topology)
	{
		switch (topology)
		{
		case TOPOLOGY_LINE:			return GL_LINES;
		case TOPOLOGY_LINE_STRIP:	return GL_LINE_STRIP;
		case TOPOLOGY_TRIANGLE:		return GL_TRIANGLES;
		case TOPOLOGY_TRIANGLE_STRIP: return GL_TRIANGLE_STRIP;
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

	void GLRenderer::CopyFrameBuffer(IGPUFrameBuffer* frameBufferFrom, IGPUFrameBuffer* frameBufferTo, Int32 srcX0, Int32 srcY0, Int32 srcX1, Int32 srcY1, Int32 dstX0, Int32 dstY0, Int32 dstX1, Int32 dstY1, UInt32 mask, ETextureFilter filter)
	{
		GLFrameBuffer* realFrameBufferFrom = static_cast<GLFrameBuffer*>(frameBufferFrom);
		GLFrameBuffer* realFrameBufferTo = static_cast<GLFrameBuffer*>(frameBufferTo);

		glBlitNamedFramebuffer(realFrameBufferFrom->getFBO(), realFrameBufferTo->getFBO(), srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, getRenderBitMask(mask), getTextureFilter(filter));
	}

	void GLRenderer::SetVsync(bool bEnabled)
	{
		glfwSwapInterval(bEnabled ? 1 : 0);
	}

	void GLRenderer::PushDebugGroup(const char* groupName)
	{
		glPushDebugGroup(GL_DEBUG_SOURCE_APPLICATION, currentDebugId++, -1, groupName);
	}

	void GLRenderer::PopDebugGroup()
	{
		glPopDebugGroup();
	}

	void GLRenderer::SetRenderBlendState(IGPUBlendState* renderBlendState)
	{
		GLBlendState* state = static_cast<GLBlendState*>(renderBlendState);
		if (state->m_blendEnabled)
		{
			glEnable(GL_BLEND);

			glBlendFunc(state->m_sourceBlendFactor, state->m_destBlendFactor);
			glBlendEquation(state->m_blendMode);
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	void GLRenderer::SetBlendColorRef(Float32 red, Float32 green, Float32 blue, Float32 alpha)
	{
		glBlendColor(red, green, blue, alpha);
	}

	void GLRenderer::SetRenderDepthStencilState(IGPUDepthStencilState* renderDepthStencilState)
	{
		GLDepthStencilState* state = static_cast<GLDepthStencilState*>(renderDepthStencilState);

		if (state->m_depthEnabled)
		{
			glEnable(GL_DEPTH_TEST);
		}
		else
		{
			glDisable(GL_DEPTH_TEST);
		}

		glDepthFunc(state->m_depthTestFunc);

		if (state->m_stencilEnabled)
		{
			glEnable(GL_STENCIL_TEST);
		}
		else
		{
			glDisable(GL_STENCIL_TEST);
		}

		glStencilFunc(state->m_stencilTestFunc, state->m_stencilRefMask, state->m_stencilMask);
		glStencilMask(state->m_stencilWriteMask);

		glStencilOp(state->m_stencilFailOp, 
			state->m_depthFailOp, 
			state->m_depthStencilPassOp);

		glDepthMask(state->m_enableDepthMask ? GL_TRUE : GL_FALSE);
	}

	void GLRenderer::SetRenderRasterizerState(IGPURasterizerState* renderRasterizerState)
	{
		GLRasterizerState* state = static_cast<GLRasterizerState*>(renderRasterizerState);
		glFrontFace(state->m_front);

		if (state->m_cullMode == 0)
		{
			glDisable(GL_CULL_FACE);
		}
		else
		{
			glEnable(GL_CULL_FACE);
			glCullFace(state->m_cullMode);
		}

		glPolygonMode(GL_FRONT_AND_BACK, state->m_fillMode);
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

	void GLRenderer::FlushCommands()
	{
		glFlush();
	}

	void GLRenderer::FinishCommands()
	{
		glFinish();
	}

	ZE::UInt32 GLRenderer::BeginQuery(ERenderQueryType type)
	{
		while (currentQueryId >= m_queryIds.size())
		{
			GLuint newId;
			glGenQueries(1, &newId);
			m_queryIds.push_back(newId);
		}

		glBeginQuery(getRenderQuery(type), m_queryIds[currentQueryId++]);
		
		return currentQueryId;
	}

	bool GLRenderer::CheckQueryAvailable(UInt32 queryId)
	{
		GLuint isAvailable = GL_FALSE;
		glGetQueryObjectuiv(m_queryIds[queryId-1], GL_QUERY_RESULT_AVAILABLE, &isAvailable);
		return isAvailable;
	}

	void GLRenderer::RetreiveQueryResult(ERenderQueryType type, UInt32 queryId, UInt32* result)
	{
		GLuint val;
		glGetQueryObjectuiv(m_queryIds[queryId-1], GL_QUERY_RESULT, &val);
		*result = val;
	}

	void GLRenderer::EndQuery(ERenderQueryType type, UInt32 queryId)
	{
		glEndQuery(getRenderQuery(type));
	}

	void GLRenderer::MaskColor(bool redMask, bool greenMask, bool blueMask, bool alphaMask)
	{
		glColorMask(redMask ? GL_TRUE : GL_FALSE, greenMask ? GL_TRUE : GL_FALSE, blueMask ? GL_TRUE : GL_FALSE, alphaMask ? GL_TRUE : GL_FALSE);
	}

}