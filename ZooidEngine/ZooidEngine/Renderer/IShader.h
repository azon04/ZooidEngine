#ifndef __Z_SHADER__
#define __Z_SHADER__

#include "BufferLayout.h"

// Inner engine includes
#include "../Utils/Array.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"
#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"

#define Z_SHADER_CHAIN_SIMPLE 0
#define Z_SHADER_CHAIN_3D_DEFAULT 1
#define Z_SHADER_CHAIN_3D_DEFAULT_COLOR 2
#define Z_SHADER_CHAIN_3D_DEFAULT_LIT 3

#define Z_SHADER_VERTEX 0
#define Z_SHADER_PIXEL 1
#define Z_SHADER_GEOMETRY 2
#define Z_SHADER_COMPUTE 3

namespace ZE {

class IGPUTexture;
class IGPUBufferData;

enum RenderTopologyEnum : UInt8
{
	TOPOLOGY_TRIANGLE,
	TOPOLOGY_POINT,
	TOPOLOGY_LINE
};

class IShader {
public:
	IShader()
	{
	}

	virtual void loadShader(const char* _shaderFilePath, UInt8 _shaderType);
	virtual void loadShaderFromBuffer(char* _shaderBuffer, size_t _bufferSize) = 0;
	virtual void release();

	UInt8 m_shaderType = Z_SHADER_VERTEX;
};

class IShaderChain {
public:
	IShaderChain()
	{
		m_topology = TOPOLOGY_TRIANGLE;
	}
	virtual ~IShaderChain();
	
	virtual void MakeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader);
	virtual void Release();

	void SetLayout(BufferLayout* _layout);
	virtual void Bind() = 0;
	virtual void Unbind() = 0;

	virtual void SetVec3(const char* _constName, Vector3 _value) = 0;
	virtual void SetFloat(const char* _constName, float _value) = 0;
	virtual void SetMat(const char* _constName, const Matrix4x4& _value) = 0;
	virtual void SetInt(const char* _constName, int _value) = 0;
	virtual void SetTexture(const char* _constName, IGPUTexture* _texture, Int32 _textureIndex) = 0;

	virtual void BindConstantBuffer(const char* _blockName, IGPUBufferData* _constantBuffer) = 0;
		
	BufferLayout *m_layout;
	RenderTopologyEnum m_topology;
};
};
#endif // !__Z_SHADER__

