#include "Shader.h"

#include "../ZEngine.h"
#include "FileSystem/FileReader.h"

#if Z_RENDER_OPENGL
#include <GL/glew.h>
#endif

#include <string>
#include <fstream>
#include <sstream>

namespace ZE {

	ShaderChain::~ShaderChain()
	{
		glDeleteProgram(m_GLProgram);
	}

	void ShaderChain::MakeChain(Shader* vsShader, Shader* psShader, Shader* gsShader, Shader* csShader)
	{
		// Shader Program
		m_GLProgram = glCreateProgram();
		
		if (vsShader)
		{
			ZASSERT(vsShader->m_shaderType == Z_SHADER_VERTEX, "This isn't Vertex Shader");
			glAttachShader(m_GLProgram, vsShader->m_GLShader);
		}

		if (psShader)
		{
			ZASSERT(psShader->m_shaderType == Z_SHADER_PIXEL, "This isn't Pixel Shader");
			glAttachShader(m_GLProgram, psShader->m_GLShader);
		}

		if (gsShader)
		{
			ZASSERT(gsShader->m_shaderType == Z_SHADER_GEOMETRY, "This isn't Geometry Shader");
			glAttachShader(m_GLProgram, gsShader->m_GLShader);
		}

		if (csShader)
		{
			ZASSERT(csShader->m_shaderType == Z_SHADER_COMPUTE, "This isn't Compute Shader");
			glAttachShader(m_GLProgram, csShader->m_GLShader);
		}

		glLinkProgram(m_GLProgram);
		// Print linking error if any
		int success;
		char infoLog[512];
		glGetProgramiv(m_GLProgram, GL_LINK_STATUS, &success);
		if (!success) {
			glGetProgramInfoLog(m_GLProgram, 512, NULL, infoLog);
			ZASSERT(false, "Shader Linking Error: %s", infoLog);
		}
	}

	void ShaderChain::LoadShaderFiles(const char* _vertexShaderFile, const char* _colorShaderFile, const char* _computeShaderFile)
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
				ZASSERT(false, "Shader Compile Error: File: %s, %s", _vertexShaderFile, infoLog);
			}

			// Fragment shader compilation
			fragment = glCreateShader(GL_FRAGMENT_SHADER);
			glShaderSource(fragment, 1, &fShaderCode, NULL);
			glCompileShader(fragment);

			// Compile error if any
			glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(fragment, 512, NULL, infoLog);
				ZASSERT(false, "Shader Compile Error: File: %s, %s", _colorShaderFile, infoLog);
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
				ZASSERT(false, "Shader Linking Error: %s", infoLog);
			}

			glDeleteShader(vertex);
			glDeleteShader(fragment);
		}
	}

	void ShaderChain::Release()
	{
#if Z_RENDER_OPENGL
		if (m_GLProgram)
		{
			glDeleteProgram(m_GLProgram);
		}
#endif
	}

	void ShaderChain::SetLayout(BufferLayout* _layout)
	{
		m_layout = _layout;
	}

	void ShaderChain::Bind()
	{
		glUseProgram(this->m_GLProgram);
	}

	void ShaderChain::Unbind()
	{
		glUseProgram(0);
	}

	void ShaderChain::SetVec3(const char* _constName, Vector3 _value)
	{
#if Z_RENDER_OPENGL
		glUniform3f(getUniformPosition(_constName), _value.getX(), _value.getY(), _value.getZ());
#endif
	}

	void ShaderChain::SetFloat(const char* _constName, float _value)
	{
#if Z_RENDER_OPENGL
		glUniform1f(getUniformPosition(_constName), _value);
#endif
	}

	void ShaderChain::SetMat(const char* _constName, const Matrix4x4& _value)
	{
#if Z_RENDER_OPENGL
		glUniformMatrix4fv(getUniformPosition(_constName), 1, GL_FALSE, &_value.m_data[0][0]);
#endif
	}

	void ShaderChain::SetInt(const char* _constName, int _value)
	{
#if Z_RENDER_OPENGL
		glUniform1i(getUniformPosition(_constName), _value);
#endif
	}

	void ShaderChain::SetTexture(const char* _constName, GPUTexture* _texture, Int32 _textureIndex)
	{
#if Z_RENDER_OPENGL
		GLint pos = getUniformPosition(_constName);
		glUniform1i(getUniformPosition(_constName), _textureIndex);
#endif
	}

	void ShaderChain::BindConstantBuffer(const char* _blockName, IGPUBufferData* _constantBuffer)
	{
#if Z_RENDER_OPENGL
		GLint block_index = glGetUniformBlockIndex(m_GLProgram, _blockName);
		if (block_index >= 0)
		{
			glUniformBlockBinding(m_GLProgram, block_index, _constantBuffer->m_bindingIndex);
		}
#endif
	}

#if Z_RENDER_OPENGL
	GLint ShaderChain::getUniformPosition(const char* _varName)
	{
		GLint pos = glGetUniformLocation(m_GLProgram, _varName);
		return pos;
	}
#endif

	void Shader::loadShader(const char* _shaderFilePath, UInt8 _shaderType)
	{
		FileReader file(_shaderFilePath);
		Handle bufferHandle(sizeof(char) * (file.size() + 1));
		Int32 resultSize = file.readToBuffer(bufferHandle.getObject(), file.size());
		char* shaderText = bufferHandle.getObject<char>();
		shaderText[resultSize] = '\0';

		m_shaderType = _shaderType;
#if Z_RENDER_OPENGL
		GLenum glShaderType = GL_VERTEX_SHADER;
		switch (m_shaderType)
		{
		case Z_SHADER_VERTEX:
			glShaderType = GL_VERTEX_SHADER;
			break;
		case Z_SHADER_PIXEL:
			glShaderType = GL_FRAGMENT_SHADER;
			break;
		case Z_SHADER_GEOMETRY:
			glShaderType = GL_GEOMETRY_SHADER;
			break;
		case Z_SHADER_COMPUTE:
			glShaderType = GL_COMPUTE_SHADER;
			break;
		}

		m_GLShader = glCreateShader(glShaderType);
		glShaderSource(m_GLShader, 1, &shaderText, NULL);
		glCompileShader(m_GLShader);

		int success;
		char infoLog[512];
		glGetShaderiv(m_GLShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(m_GLShader, 512, NULL, infoLog);
			ZASSERT(false, "Shader Compile Error: File: %s, %s", _shaderFilePath, infoLog);
		}
#endif

		bufferHandle.release();

	}

	void Shader::release()
	{
#if Z_RENDER_OPENGL
		if (m_GLShader > 0)
		{
			glDeleteShader(m_GLShader);
		}
#endif
	}

}