#include "GLRenderer.h"

#include "../ZEngine.h"

namespace ZE {

	void GLRenderer::Setup()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

		m_window = glfwCreateWindow(WIDTH, HEIGHT, "ZEngine - Windows", nullptr, nullptr);
		if (!m_window) {
			glfwTerminate();
			ZASSERT(true, "Failed to create GLFW window");
			return;
		}
		glfwMakeContextCurrent(m_window);

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
	}

	void GLRenderer::BeginRender()
	{
		glfwPollEvents();
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
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLRenderer::Draw(ShaderAction* shaderAction)
	{
		shaderAction->m_shader->Bind();
		
		for (int i = 0; i < shaderAction->m_shaderVariables.length(); i++)
		{
			ShaderVariable& shaderVariable = shaderAction->m_shaderVariables[i];
			switch (shaderVariable.m_varType)
			{
			case SHADER_VAR_TYPE_FLOAT:
				shaderAction->m_shader->SetFloat(shaderVariable.m_varName, shaderVariable.float_value);
				break;
			case SHADER_VAR_TYPE_INT:
				//shaderAction->m_shader->set(shaderVariable.m_varName, shaderVariable.int_value);
				break;
			case SHADER_VAR_TYPE_VECTOR3:
				shaderAction->m_shader->SetVec3(shaderVariable.m_varName, shaderVariable.vec3_value);
				break;
			case SHADER_VAR_TYPE_MATRIX:
				shaderAction->m_shader->SetMat(shaderVariable.m_varName, shaderVariable.mat_value);
				break;
			}
		}
		shaderAction->m_bufferArray->Bind();
		if (shaderAction->m_bufferArray->m_bUsingIndexBuffer) {
			glDrawElements(GL_TRIANGLES, shaderAction->m_vertexSize, GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawArrays(GL_TRIANGLES, 0, shaderAction->m_vertexSize);
		}
		
		shaderAction->m_bufferArray->Unbind();
		shaderAction->m_shader->Unbind();
	}

	bool GLRenderer::IsClose()
	{
		return glfwWindowShouldClose(m_window) == 1;
	}

}