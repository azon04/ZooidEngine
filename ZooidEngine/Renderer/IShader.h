#ifndef __Z_SHADER__
#define __Z_SHADER__

#include "BufferLayout.h"

// Inner engine includes
#include "Utils/Array.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"

#define Z_SHADER_CHAIN_SIMPLE 0
#define Z_SHADER_CHAIN_3D_DEFAULT 1
#define Z_SHADER_CHAIN_3D_DEFAULT_COLOR 2
#define Z_SHADER_CHAIN_3D_DEFAULT_LIT 3
#define Z_SHADER_CHAIN_3D_DEFAULT_LIT_BLEND 4
#define Z_SHADER_CHAIN_3D_HIGHLIGHT 5
#define Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT 6
#define Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT_BLEND 7

namespace ZE 
{

class IGPUTexture;
class IGPUBufferData;

enum ShaderType : UInt8
{
	SHADER_VERTEX = 0,
	SHADER_PIXEL = 1,
	SHADER_GEOMETRY = 2,
	SHADER_COMPUTE = 3,
	SHADER_MAX
};

enum RenderTopologyEnum : UInt8
{
	TOPOLOGY_TRIANGLE,
	TOPOLOGY_POINT,
	TOPOLOGY_LINE
};

class IShader 
{
public:
	IShader()
	{
	}

	// Load shader from file
	virtual void loadShader(const char* _shaderFilePath, ShaderType _shaderType);
	
	// Load shader from buffer
	virtual void loadShaderFromBuffer(char* _shaderBuffer, size_t _bufferSize) = 0;
	
	// Release shader
	virtual void release();

	FORCEINLINE ShaderType getShaderType() const { return m_shaderType; }

protected:
	ShaderType m_shaderType = SHADER_VERTEX;
};

class IShaderChain 
{
	friend class ShaderManager;

public:
	IShaderChain()
	{
		m_topology = TOPOLOGY_TRIANGLE;
	}
	virtual ~IShaderChain();
	
	// make shader chain from vsShader, psShader, geometry shader, and compute shader
	virtual void makeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader);
	
	// Release shader chain
	virtual void release();

	void setLayout(BufferLayout* _layout);
	
	// Bind shader chain to render pipeline
	virtual void bind() = 0;

	// Unbind shader chain from render pipeline
	virtual void unbind() = 0;

	// set vec3
	virtual void setVec3(const char* _constName, Vector3 _value) = 0;

	// set float variable in shader chain
	virtual void setFloat(const char* _constName, float _value) = 0;
	
	// set Matrix4x4 variable in shader chain
	virtual void setMat(const char* _constName, const Matrix4x4& _value) = 0;
	
	// set integer variable in shader chain
	virtual void setInt(const char* _constName, int _value) = 0;
	
	// set Texture variable in shader chain
	virtual void setTexture(const char* _constName, IGPUTexture* _texture, Int32 _textureIndex) = 0;

	// Bind constant buffer to shader chain
	virtual void bindConstantBuffer(const char* _blockName, IGPUBufferData* _constantBuffer) = 0;
	
	FORCEINLINE RenderTopologyEnum getRenderTopology() const { return m_topology; }

protected:
	BufferLayout* m_layout;
	RenderTopologyEnum m_topology;
};
};

#endif

