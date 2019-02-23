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
		GLShaderChain() : m_GLProgram(0) {}

		// IShaderChain Implementation
		virtual void makeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader) override;
		virtual void release() override;

		virtual void bind() override;
		virtual void unbind() override;

		virtual void setVec2(const char* _constName, const Vector2& _value) override;
		virtual void setVec3(const char* _constName, const Vector3& _value) override;
		virtual void setVec4(const char* _constName, const Vector4& _value) override;
		virtual void setFloat(const char* _constName, float _value) override;
		virtual void setMat(const char* _constName, const Matrix4x4& _value) override;
		virtual void setInt(const char* _constName, int _value) override;
		virtual void setTexture(const char* _constName, IGPUTexture* _texture, Int32 _textureIndex) override;

		virtual void bindConstantBuffer(const char* _blockName, IGPUBufferData* _constantBuffer) override;

		// Get uniform position
		GLint getUniformPosition(const char* _varName);

	private:
		GLuint m_GLProgram;
	};

}
#endif
