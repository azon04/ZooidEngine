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

	void Reset();
	void SetShaderAndBuffer(IShaderChain* _shader, IGPUBufferArray* _bufferArray);
	void SetShaderFloatVar(const char* _name, float _value);
	void SetShaderIntVar(const char* _name, Int32 _value);
	void SetShaderVec3Var(const char* _name, const Vector3& _value);
	void SetShaderMatVar(const char* _name, const Matrix4x4& _value);
	void SetData(const char* _name, void* _data);
	void SetShaderTextureVar(const char* _name, IGPUTexture* _texture, Int32 _texture_index);
	void SetType(ZE::UInt16 _shaderActionType);
	void SetConstantsBlockBuffer(const char* _name, IGPUBufferData* _constantBlockBuffer);
	
	void GetShaderMatVar(const char* _name, Matrix4x4& _value);

	void AddShaderFeature(UInt32 _feature, bool _enabled);

	ZE::Int32 m_vertexSize;
	ZE::UInt16 m_shaderActionType;
	
	IShaderChain* m_shader;
	IGPUBufferArray* m_bufferArray;
	Array<ShaderVariable, true> m_shaderVariables;
	Array<ShaderFeature, true> m_shaderFeatures;
};

// Shader Action Helper; Representative Name of function
void EnableAndSetDepthFunction(ShaderAction& shaderAction, RendererCompareFunc func);
void EnableAndSetStencilFunc(ShaderAction& shaderAction, RendererCompareFunc func, Int32 ref, UInt32 refMask, UInt32 stencilWriteMask);
void EnableAndSetBlendFunc(ShaderAction& shaderAction, RendererBlendFactor sourceBlenFactor, RendererBlendFactor dstBlendFactor);
void EnableAndSetFaceCull(ShaderAction& shaderAction, FaceFrontOrder faceOrder, CullFace cullFace);

};
#endif // __Z_SHADER_ACTION__
