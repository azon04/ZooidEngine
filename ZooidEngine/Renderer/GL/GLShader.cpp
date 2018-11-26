#include "GLShader.h"

#include "Renderer/IGPUBufferData.h"

namespace ZE
{
	void GLShader::loadShaderFromBuffer(char* _shaderBuffer, size_t _bufferSize)
	{
		GLenum glShaderType = GL_VERTEX_SHADER;
		switch (m_shaderType)
		{
		case SHADER_VERTEX:
			glShaderType = GL_VERTEX_SHADER;
			break;
		case SHADER_PIXEL:
			glShaderType = GL_FRAGMENT_SHADER;
			break;
		case SHADER_GEOMETRY:
			glShaderType = GL_GEOMETRY_SHADER;
			break;
		case SHADER_COMPUTE:
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

	void GLShaderChain::makeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader)
	{
		IShaderChain::makeChain(vsShader, psShader, gsShader, csShader);

		// Shader Program
		m_GLProgram = glCreateProgram();

		if (vsShader)
		{
			GLShader* vsGLShader = static_cast<GLShader*>(vsShader);
			ZASSERT(vsShader->getShaderType() == SHADER_VERTEX, "This isn't Vertex Shader");
			glAttachShader(m_GLProgram, vsGLShader->getGLShader());
		}

		if (psShader)
		{
			GLShader* psGLShader = static_cast<GLShader*>(psShader);
			ZASSERT(psShader->getShaderType() == SHADER_PIXEL, "This isn't Pixel Shader");
			glAttachShader(m_GLProgram, psGLShader->getGLShader());
		}

		if (gsShader)
		{
			GLShader* gsGLShader = static_cast<GLShader*>(gsShader);
			ZASSERT(gsShader->getShaderType() == SHADER_GEOMETRY, "This isn't Geometry Shader");
			glAttachShader(m_GLProgram, gsGLShader->getGLShader());
		}

		if (csShader)
		{
			GLShader* csGLShader = static_cast<GLShader*>(csShader);
			ZASSERT(csShader->getShaderType() == SHADER_COMPUTE, "This isn't Compute Shader");
			glAttachShader(m_GLProgram, csGLShader->getGLShader());
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

	void GLShaderChain::release()
	{
		IShaderChain::release();

		if (m_GLProgram)
		{
			glDeleteProgram(m_GLProgram);
		}
	}

	void GLShaderChain::bind()
	{
		glUseProgram(m_GLProgram);
	}

	void GLShaderChain::unbind()
	{
		glUseProgram(0);
	}

	void GLShaderChain::setVec2(const char* _constName, const Vector2& _value)
	{
		glUniform2f(getUniformPosition(_constName), _value.getX(), _value.getY());
	}

	void GLShaderChain::setVec3(const char* _constName, const Vector3& _value)
	{
		glUniform3f(getUniformPosition(_constName), _value.getX(), _value.getY(), _value.getZ());
	}

	void GLShaderChain::setVec4(const char* _constName, const Vector4& _value)
	{
		glUniform4f(getUniformPosition(_constName), _value.getX(), _value.getY(), _value.getZ(), _value.getW());
	}

	void GLShaderChain::setFloat(const char* _constName, float _value)
	{
		glUniform1f(getUniformPosition(_constName), _value);
	}

	void GLShaderChain::setMat(const char* _constName, const Matrix4x4& _value)
	{
		glUniformMatrix4fv(getUniformPosition(_constName), 1, GL_FALSE, &_value.m_data[0][0]);
	}

	void GLShaderChain::setInt(const char* _constName, int _value)
	{
		glUniform1i(getUniformPosition(_constName), _value);
	}

	void GLShaderChain::setTexture(const char* _constName, IGPUTexture* _texture, Int32 _textureIndex)
	{
		glUniform1i(getUniformPosition(_constName), _textureIndex);
		glActiveTexture(GL_TEXTURE0 + _textureIndex);
	}

	void GLShaderChain::bindConstantBuffer(const char* _blockName, IGPUBufferData* _constantBuffer)
	{
		GLint block_index = glGetUniformBlockIndex(m_GLProgram, _blockName);
		if (block_index >= 0)
		{
			glUniformBlockBinding(m_GLProgram, block_index, _constantBuffer->getBindingIndex());
		}
	}

	GLint GLShaderChain::getUniformPosition(const char* _varName)
	{
		GLint pos = glGetUniformLocation(m_GLProgram, _varName);
		return pos;
	}

}