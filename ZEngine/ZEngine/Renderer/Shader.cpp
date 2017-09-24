#include "Shader.h"

#include "../ZEngine.h"

// #OPENGL Specific
#include <GL/glew.h>

#include <string>
#include <fstream>
#include <sstream>

namespace ZE {

	Shader::~Shader()
	{
		glDeleteProgram(m_GLProgram);
	}

	void Shader::LoadShaderFiles(const char* _vertexShaderFile, const char* _colorShaderFile, const char* _computeShaderFile)
	{
		// #OPENGL Specific
		if (_vertexShaderFile != NULL && _colorShaderFile != NULL) {
			std::string vertexCode;
			std::string fragmentCode;
			std::ifstream vShaderFile;
			std::ifstream fShaderFile;
			// ensure ifstream object can throw exception
			vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
			try {
				// Open files
				vShaderFile.open(_vertexShaderFile);
				fShaderFile.open(_colorShaderFile);

				std::stringstream vShaderStream, fShaderStream;
				vShaderStream << vShaderFile.rdbuf();
				fShaderStream << fShaderFile.rdbuf();

				// Close handlers
				vShaderFile.close();
				fShaderFile.close();

				// Convert to string
				vertexCode = vShaderStream.str();
				fragmentCode = fShaderStream.str();
			}
			catch (std::ifstream::failure e) {
				ZASSERT(true, "Shader files not exist");
			}

			const char* vShaderCode = vertexCode.c_str();
			const char* fShaderCode = fragmentCode.c_str();

			// Compile shader
			unsigned int vertex, fragment;
			int success;
			char infoLog[512];

			// Vertex shader compilation
			vertex = glCreateShader(GL_VERTEX_SHADER);
			glShaderSource(vertex, 1, &vShaderCode, NULL);
			glCompileShader(vertex);

			// Compile error if any
			glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(vertex, 512, NULL, infoLog);
				ZASSERT(true, infoLog);
			}

			// Fragment shader compilation
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);

			// Compile error if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				ZASSERT(fragment, infoLog);
			}


			// Shader Program
			this->m_GLProgram = glCreateProgram();
			glAttachShader(m_GLProgram, vertex);
			glAttachShader(m_GLProgram, fragment);
			glLinkProgram(m_GLProgram);
			// Print linking error if any
			glGetProgramiv(m_GLProgram, GL_LINK_STATUS, &success);
			if (!success) {
				glGetProgramInfoLog(m_GLProgram, 512, NULL, infoLog);
				ZASSERT(true, infoLog);
			}

			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}
	}

	void Shader::SetLayout(BufferLayout* _layout)
	{
		m_layout = _layout;
	}

	void Shader::Bind()
	{
		glUseProgram(this->m_GLProgram);
	}

	void Shader::Unbind()
	{
		glUseProgram(0);
	}

	void Shader::SetVec3(const char* _constName, Vector3 _value)
	{
#if Z_RENDER_OPENGL
		glUniform3f(getUniformPosition(_constName), _value.getX(), _value.getY(), _value.getZ());
#endif
	}

	void Shader::SetFloat(const char* _constName, float _value)
	{
#if Z_RENDER_OPENGL
		glUniform1f(getUniformPosition(_constName), _value);
#endif
	}

	void Shader::SetMat(const char* _constName, Matrix4x4 _value)
	{
#if Z_RENDER_OPENGL
		glUniformMatrix4fv(getUniformPosition(_constName), 1, GL_TRUE, (float*)(_value.m_data));
#endif
	}

#if Z_RENDER_OPENGL
	GLint Shader::getUniformPosition(const char* _varName)
	{
		return glGetUniformLocation(m_GLProgram, _varName);
	}
#endif

	ShaderManager::ShaderManager()
	{

	}

	void ShaderManager::Init()
	{
		Handle handle("SHADER MANAGER", sizeof(ShaderManager));
		m_instance = new(handle) ShaderManager;
		m_instance->InitShaders();
	}

	ShaderManager* ShaderManager::m_instance = nullptr;

	void ShaderManager::Destroy()
	{
		m_instance->DestroyShaders();
	}

	ShaderManager* ShaderManager::getInstance()
	{
		return m_instance;
	}

	void ShaderManager::InitShaders()
	{
		/*** 
		* Initialize all shaders
		***/

#if Z_RENDER_OPENGL
		m_shaders.reset(2);
		{
			Handle handle("Simple Shader", sizeof(Shader));
			Shader* simpleGLShader = new(handle) Shader;
			simpleGLShader->LoadShaderFiles("Shaders/TestGLVertexShader.vs", "Shaders/TestGLFragmentShader.frag", nullptr);

			m_shaders.push_back(simpleGLShader);
		}
		{
			Handle handle("Default GL Shader", sizeof(Shader));
			Shader* defaultGLShader = new(handle) Shader;
			defaultGLShader->LoadShaderFiles("Shaders/DefaultGLSimple.vs", "Shaders/DefaultGLSimple.frag", nullptr);

			m_shaders.push_back(defaultGLShader);
		}
#endif

	}

	void ShaderManager::DestroyShaders()
	{
		m_shaders.clear();
	}

}