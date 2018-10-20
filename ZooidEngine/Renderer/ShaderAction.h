#ifndef __Z_SHADER_ACTION__
#define __Z_SHADER_ACTION__

#include "Utils/PrimitiveTypes.h"

#include "IRenderer.h"
#include "IShader.h"
#include "IGPUBufferArray.h"
#include "IGPUTexture.h"

#define SHADER_VAR_TYPE_FLOAT 0
#define SHADER_VAR_TYPE_INT 1
#define SHADER_VAR_TYPE_VECTOR3 2
#define SHADER_VAR_TYPE_MATRIX 3
#define SHADER_VAR_TYPE_TEXTURE 4
#define SHADER_VAR_TYPE_BLOCK_BUFFER 5
#define SHADER_VAR_TYPE_DATA 6

#define SHADER_ACTION_DRAW 0
#define SHADER_ACTION_SETGLOBAL 1

namespace ZE 
{

struct ShaderVariable
{
	char VarName[32];
	UInt8 VarType;

	union
	{
		Matrix4x4 Mat_value;
		Vector3 Vec3_value;
		float Float_value;
		Int32 Int_value;
		void* Data_value;

		struct tTexture 
		{
			IGPUTexture* Texture_data;
			Int32 Texture_index;
		} texture_value;

		IGPUBufferData* Constant_buffer;
	};

	ShaderVariable() : Mat_value(Matrix4x4())
	{}

	ShaderVariable(const ShaderVariable& _other);

	~ShaderVariable() {}
};

union ShaderFeatureVar
{
	UInt32 Uint_value;
	Int32 Int_value;
	void* pAddress;
};

struct ShaderFeature
{
	ShaderFeature() {}

	ShaderFeature(const ShaderFeature& other)
	{
		m_rendererFeature = other.m_rendererFeature;
		m_bFeatureEnabled = other.m_bFeatureEnabled;
		m_shaderFeatureVar = other.m_shaderFeatureVar;
	}

	ShaderFeature& operator=(const ShaderFeature& other)
	{
		m_rendererFeature = other.m_rendererFeature;
		m_bFeatureEnabled = other.m_bFeatureEnabled;
		m_shaderFeatureVar = other.m_shaderFeatureVar;
		return *this;
	}

	UInt32 m_rendererFeature;
	bool m_bFeatureEnabled;

	Array<ShaderFeatureVar, true> m_shaderFeatureVar;
};

class ShaderAction 
{
public:

	ShaderAction();
	ShaderAction(IShaderChain* shader);

	// reset/clear shader action values
	void reset();

	// Set shader chain
	void setShaderChain(IShaderChain* _shader);

	// Set Buffer
	void setBuffer(IGPUBufferArray* _bufferArray);

	// set main shader and buffer
	void setShaderAndBuffer(IShaderChain* _shader, IGPUBufferArray* _bufferArray);
	
	// set shader variable float value
	void setShaderFloatVar(const char* _name, float _value);
	
	// set shader variable integer value
	void setShaderIntVar(const char* _name, Int32 _value);

	// set shader variable vector value
	void setShaderVec3Var(const char* _name, const Vector3& _value);

	// set shader variable matrix value
	void setShaderMatVar(const char* _name, const Matrix4x4& _value);

	// set shader variable value / basically copy buffer to variable
	void setData(const char* _name, void* _data);

	// set Shader Texture variable value
	void setShaderTextureVar(const char* _name, IGPUTexture* _texture, Int32 _texture_index);

	// set type of shader action
	// SHADER_ACTION_DRAW: for drawing
	// SHADER_ACTION_SETGLOBAL: set global buffer value
	void setType(ZE::UInt16 _shaderActionType);

	// set constants block buffer variable
	void setConstantsBlockBuffer(const char* _name, IGPUBufferData* _constantBlockBuffer);
	
	// get shader matrix variable value
	void getShaderMatVar(const char* _name, Matrix4x4& _value);

	// disable/enable ShaderFeature, see ShaderFeature
	void addShaderFeature(UInt32 _feature, bool _enabled);

	// Override the vertex size (so it will not draw all vertex inside the buffer array)
	void setVertexSize(Int32 _overrideSize) { m_vertexSize = _overrideSize; }

	// Set whether the object/buffer is skinned (skeleton)
	void setSkin(bool bSkin) { m_bSkin = bSkin; }
	
	// Check if the ShaderAction contain Skin vertices/data
	FORCEINLINE bool isSkin() const { return m_bSkin; }

	FORCEINLINE Int32 getVertexSize() const { return m_vertexSize; }
	FORCEINLINE UInt16 getShaderActionType() const { return m_shaderActionType; }

	FORCEINLINE IShaderChain* getShaderChain() const { return m_shader; }
	FORCEINLINE IGPUBufferArray* getBufferArray() const { return m_bufferArray; }

	friend void EnableAndSetDepthFunction(ShaderAction& shaderAction, ERendererCompareFunc func);
	friend void EnableAndSetStencilFunc(ShaderAction& shaderAction, ERendererCompareFunc func, Int32 ref, UInt32 refMask, UInt32 stencilWriteMask);
	friend void EnableAndSetBlendFunc(ShaderAction& shaderAction, ERendererBlendFactor sourceBlenFactor, ERendererBlendFactor dstBlendFactor);
	friend void EnableAndSetFaceCull(ShaderAction& shaderAction, EFaceFrontOrder faceOrder, ECullFace cullFace);

public:
	Array<ShaderFeature, true> m_shaderFeatures;
	Array<ShaderVariable, true> m_shaderVariables;

private:
	ZE::Int32 m_vertexSize;
	ZE::UInt16 m_shaderActionType;
	
	IShaderChain* m_shader;
	IGPUBufferArray* m_bufferArray;
	bool m_bSkin = false;
};


};
#endif
