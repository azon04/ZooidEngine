#include "GLShader.h"

#include "Renderer/IGPUBufferData.h"

namespace ZE
{

	void GLShader::loadShaderFromBuffer(char* _shaderBuffer, size_t _bufferSize)
	{
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
		glShaderSource(m_GLShader, 1, &_shaderBuffer, NULL);
		glCompileShader(m_GLShader);

		int success;
		char infoLog[512];
		glGetShaderiv(m_GLShader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(m_GLShader, 512, NULL, infoLog);
			ZASSERT(false, "Shader Compile Error: %s", infoLog);
		}
	}

	void GLShader::release()
	{
		if (m_GLShader > 0)
		{
			glDeleteShader(m_GLShader);
		}
	}

	void GLShaderChain::MakeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader)
	{
		IShaderChain::MakeChain(vsShader, psShader, gsShader, csShader);

		// Shader Program
		m_GLProgram = glCreateProgram();

		if (vsShader)
		{
			GLShader* vsGLShader = static_cast<GLShader*>(vsShader);
			ZASSERT(vsShader->m_shaderType == Z_SHADER_VERTEX, "This isn't Vertex Shader");
			glAttachShader(m_GLProgram, vsGLShader->m_GLShader);
		}

		if (psShader)
		{
			GLShader* psGLShader = static_cast<GLShader*>(psShader);
			ZASSERT(psShader->m_shaderType == Z_SHADER_PIXEL, "This isn't Pixel Shader");
			glAttachShader(m_GLProgram, psGLShader->m_GLShader);
		}

		if (gsShader)
		{
			GLShader* gsGLShader = static_cast<GLShader*>(gsShader);
			ZASSERT(gsShader->m_shaderType == Z_SHADER_GEOMETRY, "This isn't Geometry Shader");
			glAttachShader(m_GLProgram, gsGLShader->m_GLShader);
		}

		if (csShader)
		{
			GLShader* csGLShader = static_cast<GLShader*>(csShader);
			ZASSERT(csShader->m_shaderType == Z_SHADER_COMPUTE, "This isn't Compute Shader");
			glAttachShader(m_GLProgram, csGLShader->m_GLShader);
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

	void GLShaderChain::Release()
	{
		IShaderChain::Release();

		if (m_GLProgram)
		{
			glDeleteProgram(m_GLProgram);
		}
	}

	void GLShaderChain::Bind()
	{
		glUseProgram(m_GLProgram);
	}

	void GLShaderChain::Unbind()
	{
		glUseProgram(0);
	}

	void GLShaderChain::SetVec3(const char* _constName, Vector3 _value)
	{
		glUniform3f(getUniformPosition(_constName), _value.getX(), _value.getY(), _value.getZ());
	}

	void GLShaderChain::SetFloat(const char* _constName, float _value)
	{
		glUniform1f(getUniformPosition(_constName), _value);
	}

	void GLShaderChain::SetMat(const char* _constName, const Matrix4x4& _value)
	{
		glUniformMatrix4fv(getUniformPosition(_constName), 1, GL_FALSE, &_value.m_data[0][0]);
	}

	void GLShaderChain::SetInt(const char* _constName, int _value)
	{
		glUniform1i(getUniformPosition(_constName), _value);
	}

	void GLShaderChain::SetTexture(const char* _constName, IGPUTexture* _texture, Int32 _textureIndex)
	{
		glUniform1i(getUniformPosition(_constName), _textureIndex);
	}

	void GLShaderChain::BindConstantBuffer(const char* _blockName, IGPUBufferData* _constantBuffer)
	{
		GLint block_index = glGetUniformBlockIndex(m_GLProgram, _blockName);
		if (block_index >= 0)
		{
			glUniformBlockBinding(m_GLProgram, block_index, _constantBuffer->m_bindingIndex);
		}
	}

	GLint GLShaderChain::getUniformPosition(const char* _varName)
	{
		GLint pos = glGetUniformLocation(m_GLProgram, _varName);
		return pos;
	}

}