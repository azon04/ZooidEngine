#ifndef __Z_SHADER_ACTION__
#define __Z_SHADER_ACTION__

#include "../Utils/PrimitiveTypes.h"

#include "Shader.h"
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

namespace ZE {

struct ShaderVariable
{
	char m_varName[32];
	UInt8 m_varType;

	union
	{
		Matrix4x4 mat_value;
		Vector3 vec3_value;
		float float_value;
		Int32 int_value;
		void* data_value;

		struct tTexture 
		{
			IGPUTexture* texture_data;
			Int32 texture_index;
		} texture_value;

		IGPUBufferData* constant_buffer;
	};

	ShaderVariable() : mat_value(Matrix4x4())
	{}

	ShaderVariable(const ShaderVariable& _other);

	~ShaderVariable() {}
};

class ShaderAction {
public:

	ShaderAction();
	ShaderAction(ShaderChain* shader);

	void Reset();
	void SetShaderAndBuffer(ShaderChain* _shader, IGPUBufferArray* _bufferArray);
	void SetShaderFloatVar(const char* _name, float _value);
	void SetShaderIntVar(const char* _name, Int32 _value);
	void SetShaderVec3Var(const char* _name, const Vector3& _value);
	void SetShaderMatVar(const char* _name, const Matrix4x4& _value);
	void SetData(const char* _name, void* _data);
	void SetShaderTextureVar(const char* _name, IGPUTexture* _texture, Int32 _texture_index);
	void SetType(ZE::UInt16 _shaderActionType);
	void SetConstantsBlockBuffer(const char* _name, IGPUBufferData* _constantBlockBuffer);

	ZE::Int32 m_vertexSize;
	ZE::UInt16 m_shaderActionType;
	
	ShaderChain* m_shader;
	IGPUBufferArray* m_bufferArray;
	Array<ShaderVariable, true> m_shaderVariables;
};
};
#endif // __Z_SHADER_ACTION__
