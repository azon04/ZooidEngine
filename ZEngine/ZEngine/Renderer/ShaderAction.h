#ifndef __Z_SHADER_ACTION__
#define __Z_SHADER_ACTION__

#include "../Utils/PrimitiveTypes.h"

#include "Shader.h"
#include "GPUBufferArray.h"

#define SHADER_VAR_TYPE_FLOAT 0
#define SHADER_VAR_TYPE_INT 1
#define SHADER_VAR_TYPE_VECTOR3 2
#define SHADER_VAR_TYPE_MATRIX 3

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
	};

	ShaderVariable() : mat_value(Matrix4x4())
	{}

	ShaderVariable(const ShaderVariable& _other);

	~ShaderVariable() {}
};

class ShaderAction {
public:

	ShaderAction();
	ShaderAction(Shader* shader);

	void SetShaderAndBuffer(Shader* _shader, GPUBufferArray* _bufferArray);
	void SetShaderFloatVar(const char* _name, float _value);
	void SetShaderIntVar(const char* _name, Int32 _value);
	void SetShaderVec3Var(const char* _name, const Vector3& _value);
	void SetShaderMatVar(const char* _name, const Matrix4x4& _value);

	ZE::Int32 m_vertexSize;

	Shader* m_shader;
	GPUBufferArray* m_bufferArray;
	Array<ShaderVariable, true> m_shaderVariables;
};
};
#endif // __Z_SHADER_ACTION__
