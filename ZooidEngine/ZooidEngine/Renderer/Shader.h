#ifndef __Z_SHADER__
#define __Z_SHADER__

#include "BufferLayout.h"

// Inner engine includes
#include "../Utils/Array.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"
#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"

// Outer engine includes
#if Z_RENDER_OPENGL
#include <GL/glew.h>
#endif 

#define Z_SHADER_CHAIN_SIMPLE 0
#define Z_SHADER_CHAIN_3D_DEFAULT 1
#define Z_SHADER_CHAIN_3D_DEFAULT_COLOR 2
#define Z_SHADER_CHAIN_3D_DEFAULT_LIT 3

#define Z_SHADER_VERTEX 0
#define Z_SHADER_PIXEL 1
#define Z_SHADER_GEOMETRY 2
#define Z_SHADER_COMPUTE 3

namespace ZE {

class GPUTexture;
class GPUBufferData;

enum RenderTopologyEnum : UInt8
{
	TOPOLOGY_TRIANGLE,
	TOPOLOGY_POINT,
	TOPOLOGY_LINE
};

class Shader {
public:
	Shader()
	{
#if Z_RENDER_OPENGL
		m_GLShader = 0;
#endif
	}

	void loadShader(const char* _shaderFilePath, UInt8 _shaderType);
	void release();

	UInt8 m_shaderType = Z_SHADER_VERTEX;

#if Z_RENDER_OPENGL
	GLuint m_GLShader;
#endif
};

class ShaderChain {
public:
	ShaderChain()
	{
#if Z_RENDER_OPENGL
		m_GLProgram = 0;
#endif
		m_topology = TOPOLOGY_TRIANGLE;
	}
	virtual ~ShaderChain();
	
	void MakeChain(Shader* vsShader, Shader* psShader, Shader* gsShader, Shader* csShader);
	void LoadShaderFiles(const char* _vertexShaderFile, const char* _colorShaderFile, const char* _computeShaderFile);
	void Release();

	void SetLayout(BufferLayout* _layout);
	void Bind();
	void Unbind();

	void SetVec3(const char* _constName, Vector3 _value);
	void SetFloat(const char* _constName, float _value);
	void SetMat(const char* _constName, Matrix4x4 _value);
	void SetInt(const char* _constName, int _value);
	void SetTexture(const char* _constName, GPUTexture* _texture, Int32 _textureIndex);

	void BindConstantBuffer(const char* _blockName, GPUBufferData* _constantBuffer);
		
	BufferLayout *m_layout;
	RenderTopologyEnum m_topology;

#if Z_RENDER_OPENGL
	GLint getUniformPosition(const char* _varName);

	GLuint m_GLProgram;
#endif
};
};
#endif // !__Z_SHADER__

