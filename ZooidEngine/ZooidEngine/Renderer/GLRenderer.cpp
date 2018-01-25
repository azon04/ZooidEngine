#include "GLRenderer.h"

#include "../ZEngine.h"

#include <cstring>

namespace ZE {

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
		glEnable(GL_DEPTH_TEST);
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
		shaderAction->m_bufferArray->Unbind();
		shaderAction->m_shader->Unbind();
	}

	bool GLRenderer::IsClose()
	{
		return glfwWindowShouldClose(m_window) == 1;
	}

}