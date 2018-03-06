#include "GLRenderer.h"

#include "ZEngine.h"

#if defined(_WIN32) || defined(_WIN64)
	#define GLFW_EXPOSE_NATIVE_WIN32
#endif

#include "GLFW/glfw3native.h"

#include <cstring>

namespace ZE {

	GLRenderer::GLRenderer()
	{
		HashFeatureToRealGLVar.put(RendererFeature::DEPTH_TEST, GL_DEPTH_TEST);
		HashFeatureToRealGLVar.put(RendererFeature::STENCIL_TEST, GL_STENCIL_TEST);
		HashFeatureToRealGLVar.put(RendererFeature::BLEND, GL_BLEND);

		HashCompareFuncToRealGLVar.put(RendererCompareFunc::ALWAYS, GL_ALWAYS);
		HashCompareFuncToRealGLVar.put(RendererCompareFunc::NEVER, GL_NEVER);
		HashCompareFuncToRealGLVar.put(RendererCompareFunc::LESS, GL_LESS);
		HashCompareFuncToRealGLVar.put(RendererCompareFunc::EQUAL, GL_EQUAL);
		HashCompareFuncToRealGLVar.put(RendererCompareFunc::LEQUAL, GL_LEQUAL);
		HashCompareFuncToRealGLVar.put(RendererCompareFunc::GREATER, GL_GREATER);
		HashCompareFuncToRealGLVar.put(RendererCompareFunc::NOTEQUAL, GL_NOTEQUAL);
		HashCompareFuncToRealGLVar.put(RendererCompareFunc::GEQUAL, GL_GEQUAL);
	}

	void GLRenderer::Setup()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
		
		m_window = glfwCreateWindow(WIDTH, HEIGHT, "ZooidEngine - Windows", nullptr, nullptr);
		if (!m_window) {
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
		if (err != GLEW_OK) {
			glfwTerminate();
			ZASSERT(false, "GL Init Error: %s", glewGetErrorString(err));
			return;
		}

		// Viewport setup
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		// Enable Depth test
		EnableFeature(RendererFeature::DEPTH_TEST);

		// Enable Stencil Buffer
		EnableFeature(RendererFeature::STENCIL_TEST);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	}

	void GLRenderer::BeginRender()
	{
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
		glStencilMask(0xFF); // Before clearning Stencil buffer
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		glStencilMask(0x00); // Disable writing to stencil buffer
	}

	void GLRenderer::ProcessDrawList(DrawList* drawList)
	{
		if (drawList->m_mainConstantBuffer)
		{
			drawList->m_mainConstantBuffer->Bind();
		}

		if (drawList->m_lightConstantBuffer)
		{
			drawList->m_lightConstantBuffer->Bind();
		}

		for (int i = 0; i < drawList->m_size; ++i)
		{
			ProcessShaderAction(&drawList->m_drawList[i]);
		}

		for (int i = 0; i < drawList->m_secondPassSize; ++i)
		{
			ProcessShaderAction(&drawList->m_secondPassDrawList[i]);
		}

		if (drawList->m_mainConstantBuffer)
		{
			drawList->m_mainConstantBuffer->UnBind();
		}

		if (drawList->m_lightConstantBuffer)
		{
			drawList->m_lightConstantBuffer->UnBind();
		}
	}

	void GLRenderer::ProcessShaderAction(ShaderAction* shaderAction)
	{
		switch (shaderAction->m_shaderActionType)
		{
		case SHADER_ACTION_DRAW:
			Draw(shaderAction);
			break;
		}
	}

	void GLRenderer::Draw(ShaderAction* shaderAction)
	{
		shaderAction->m_shader->Bind();
		
		for (int i = 0; i < shaderAction->m_shaderFeatures.length(); i++)
		{
			ShaderFeature& shaderFeature = shaderAction->m_shaderFeatures[i];
			
			bool bDefaultEnable = IsFeatureEnabled(shaderFeature.m_rendererFeature);
			
			ProcessShaderFeature(shaderFeature);

			// Save the default value for reverting the renderer feature
			shaderFeature.m_bFeatureEnabled = bDefaultEnable;
		}

		for (int i = 0; i < shaderAction->m_shaderVariables.length(); i++)
		{
			ShaderVariable& shaderVariable = shaderAction->m_shaderVariables[i];
			switch (shaderVariable.m_varType)
			{
			case SHADER_VAR_TYPE_FLOAT:
				shaderAction->m_shader->SetFloat(shaderVariable.m_varName, shaderVariable.float_value);
				break;
			case SHADER_VAR_TYPE_INT:
				shaderAction->m_shader->SetInt(shaderVariable.m_varName, shaderVariable.int_value);
				break;
			case SHADER_VAR_TYPE_VECTOR3:
				shaderAction->m_shader->SetVec3(shaderVariable.m_varName, shaderVariable.vec3_value);
				break;
			case SHADER_VAR_TYPE_MATRIX:
				shaderAction->m_shader->SetMat(shaderVariable.m_varName, shaderVariable.mat_value);
				break;
			case SHADER_VAR_TYPE_TEXTURE:
				shaderAction->m_shader->SetTexture(shaderVariable.m_varName, shaderVariable.texture_value.texture_data, shaderVariable.texture_value.texture_index);
				glActiveTexture(GL_TEXTURE0 + shaderVariable.texture_value.texture_index);
				shaderVariable.texture_value.texture_data->Bind();
				break;
			case SHADER_VAR_TYPE_BLOCK_BUFFER:
				shaderAction->m_shader->BindConstantBuffer(shaderVariable.m_varName, shaderVariable.constant_buffer);
			}
		}

		GLenum drawTopology = GL_TRIANGLES;

		switch (shaderAction->m_shader->m_topology)
		{
		case TOPOLOGY_LINE:
			drawTopology = GL_LINES;
			break;
		case TOPOLOGY_TRIANGLE:
			drawTopology = GL_TRIANGLES;
			break;
		case TOPOLOGY_POINT:
			drawTopology = GL_POINTS;
			break;
		}

		shaderAction->m_bufferArray->Bind();
		if (shaderAction->m_bufferArray->m_bUsingIndexBuffer) 
		{
			glDrawElements(drawTopology, shaderAction->m_vertexSize, GL_UNSIGNED_INT, 0);
		}
		else 
		{
			glDrawArrays(drawTopology, 0, shaderAction->m_vertexSize);
		}
		
		// Unbind textures, buffer array and shader
		for (int i = 0; i < shaderAction->m_shaderVariables.length(); i++)
		{
			ShaderVariable& shaderVariable = shaderAction->m_shaderVariables[i];
			switch (shaderVariable.m_varType)
			{
			case SHADER_VAR_TYPE_TEXTURE:
				glActiveTexture(GL_TEXTURE0 + shaderVariable.texture_value.texture_index);
				shaderVariable.texture_value.texture_data->Unbind();
				break;
			}
		}

		// Revert Render Feature
		for (int i = 0; i < shaderAction->m_shaderFeatures.length(); i++)
		{
			ShaderFeature& shaderFeature = shaderAction->m_shaderFeatures[i];
			if (shaderFeature.m_bFeatureEnabled)
			{
				EnableFeature(shaderFeature.m_rendererFeature);
				ResetFeature(shaderFeature.m_rendererFeature);
			}
			else
			{
				DisableFeature(shaderFeature.m_rendererFeature);
			}
		}

		shaderAction->m_bufferArray->Unbind();
		shaderAction->m_shader->Unbind();
	}

	bool GLRenderer::IsClose()
	{
		return glfwWindowShouldClose(m_window) == 1;
	}

	void GLRenderer::PollEvent()
	{
		glfwPollEvents();
	}

	void GLRenderer::AcquireRenderThreadOwnership()
	{
		glfwMakeContextCurrent(m_window);
		m_renderLock.lock();
	}

	void GLRenderer::ReleaseRenderThreadOwnership()
	{	
		glfwMakeContextCurrent(nullptr);
		m_renderLock.unlock();

	}

	void GLRenderer::EnableFeature(UInt32 feature)
	{
		glEnable(HashFeatureToRealGLVar[feature]);
	}

	void GLRenderer::DisableFeature(UInt32 feature)
	{
		glDisable(HashFeatureToRealGLVar[feature]);
	}

	bool GLRenderer::IsFeatureEnabled(UInt32 feature)
	{
		return glIsEnabled(HashFeatureToRealGLVar[feature]) == GL_TRUE;
	}

	void GLRenderer::ResetFeature(UInt32 feature)
	{
		if (feature == RendererFeature::DEPTH_TEST)
		{
			glDepthFunc(GL_LESS);
		}
		else if (feature == RendererFeature::STENCIL_TEST)
		{
			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0x00);
		}
	}

	void GLRenderer::ProcessShaderFeature(ShaderFeature& shaderFeature)
	{
		if (shaderFeature.m_bFeatureEnabled)
		{
			EnableFeature(shaderFeature.m_rendererFeature);

			if (shaderFeature.m_rendererFeature == RendererFeature::DEPTH_TEST)
			{

				if (shaderFeature.m_shaderFeatureVar.length() > 0)
				{
					glDepthFunc(HashCompareFuncToRealGLVar[shaderFeature.m_shaderFeatureVar[0].uint_value]);
				}
			}
			else if (shaderFeature.m_rendererFeature == RendererFeature::STENCIL_TEST)
			{
				if (shaderFeature.m_shaderFeatureVar.length() > 0)
				{
					glStencilFunc(HashCompareFuncToRealGLVar[shaderFeature.m_shaderFeatureVar[0].uint_value], 
						shaderFeature.m_shaderFeatureVar[1].int_value, 
						shaderFeature.m_shaderFeatureVar[2].uint_value);
					glStencilMask(shaderFeature.m_shaderFeatureVar[3].uint_value);
				}
			}
			else if(shaderFeature.m_rendererFeature == RendererFeature::BLEND)
			{
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
		}
		else
		{
			DisableFeature(shaderFeature.m_rendererFeature);
		}
	}

}