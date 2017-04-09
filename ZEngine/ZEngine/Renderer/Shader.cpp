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

}