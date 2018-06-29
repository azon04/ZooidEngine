#ifndef __Z_GL_SHADER__
#define __Z_GL_SHADER__

#include "Renderer/IShader.h"

#include <GL/glew.h>

namespace ZE
{
	class GLShader : public IShader
	{
	public:
		GLShader() : m_GLShader(0) {}

		// IShader implementation
		virtual void loadShaderFromBuffer(char* _shaderBuffer, size_t _bufferSize) override;
		virtual void release() override;

		FORCEINLINE  GLuint getGLShader() const { return m_GLShader; }

	private:
		GLuint m_GLShader;
		
	};

	class GLShaderChain : public IShaderChain
	{
	public:
		GLShaderChain() : m_GLProgram(0)
		{
			m_topology = TOPOLOGY_TRIANGLE;
		}

		// IShaderChain Implementation
		virtual void MakeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader);
		virtual void Release();

		virtual void Bind();
		virtual void Unbind();

		virtual void SetVec3(const char* _constName, Vector3 _value) override;
		virtual void SetFloat(const char* _constName, float _value) override;
		virtual void SetMat(const char* _constName, const Matrix4x4& _value) override;
		virtual void SetInt(const char* _constName, int _value) override;
		virtual void SetTexture(const char* _constName, IGPUTexture* _texture, Int32 _textureIndex) override;

		virtual void BindConstantBuffer(const char* _blockName, IGPUBufferData* _constantBuffer) override;

		// Get uniform position
		GLint getUniformPosition(const char* _varName);

	private:
		GLuint m_GLProgram;
	};

}
#endif
