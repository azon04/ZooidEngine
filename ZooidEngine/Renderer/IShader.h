#ifndef __Z_SHADER__
#define __Z_SHADER__

#include "BufferLayout.h"

// Inner engine includes
#include "Utils/Array.h"
#include "Math/Vector2.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"
#include "Enums.h"

#define Z_SHADER_CHAIN_SIMPLE 0
#define Z_SHADER_CHAIN_3D_DEFAULT 1
#define Z_SHADER_CHAIN_3D_DEFAULT_COLOR_LINE 2
#define Z_SHADER_CHAIN_3D_DEFAULT_LIT 3
#define Z_SHADER_CHAIN_3D_DEFAULT_LIT_BLEND 4
#define Z_SHADER_CHAIN_3D_HIGHLIGHT 5
#define Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT 6
#define Z_SHADER_CHAIN_3D_DEFAULT_SKIN_LIT_BLEND 7
#define Z_SHADER_CHAIN_WORLD_TEXT_SHADER 8
#define Z_SHADER_CHAIN_WORLD_TEXT_SHADER_SDF 9
#define Z_SHADER_CHAIN_WORLD_TEXT_SHADER_MSDF 10
#define Z_SHADER_CHAIN_SCREEN_TEXT_SHADER 11
#define Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_SDF 12
#define Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_MSDF 13
#define Z_SHADER_CHAIN_SHADOW_DEPTH 14
#define Z_SHADER_cHAIN_SHADOW_DEPTH_SKINNED 15
#define Z_SHADER_CHAIN_DRAW_SCREEN_QUAD 16

namespace ZE 
{

class IGPUTexture;
class IGPUBufferData;

class IShader 
{
public:
	IShader()
	{
	}

	// Load shader from file
	virtual void loadShader(const char* _shaderFilePath, EShaderType _shaderType);
	
	// Load shader from buffer
	virtual void loadShaderFromBuffer(char* _shaderBuffer, size_t _bufferSize) = 0;
	
	// Release shader
	virtual void release();

	FORCEINLINE EShaderType getShaderType() const { return m_shaderType; }

protected:
	EShaderType m_shaderType = SHADER_VERTEX;
};

class IShaderChain 
{
	friend class ShaderManager;

public:
	IShaderChain() {}
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

	// set vec2
	virtual void setVec2(const char* _constName, const Vector2& _value) = 0;

	// set vec3
	virtual void setVec3(const char* _constName, const Vector3& _value) = 0;

	// set vec4
	virtual void setVec4(const char* _constName, const Vector4& _value) = 0;

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

protected:
	BufferLayout* m_layout;
};
};

#endif

