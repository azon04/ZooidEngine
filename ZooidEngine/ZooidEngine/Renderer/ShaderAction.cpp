#include "ShaderAction.h"

#include "../Utils/StringFunc.h"

#include <cstring>

namespace ZE {

	ShaderAction::ShaderAction()
	{
		m_shaderActionType = SHADER_ACTION_DRAW;
	}

	ShaderAction::ShaderAction(Shader* shader)
	{
		m_shader = shader;
	}

	void ShaderAction::Reset()
	{
		m_shader = nullptr;
		m_shaderVariables.clear();
		m_bufferArray = nullptr;
		m_shaderActionType = SHADER_ACTION_DRAW;
		m_vertexSize = 0;
	}

	void ShaderAction::SetShaderAndBuffer(Shader* _shader, GPUBufferArray* _bufferArray)
	{
		m_shader = _shader;
		m_bufferArray = _bufferArray;
	}

	void ShaderAction::SetShaderFloatVar(const char* _name, float _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.m_varName, _name, 32);
		shaderVariable.m_varType = SHADER_VAR_TYPE_FLOAT;
		shaderVariable.float_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::SetShaderIntVar(const char* _name, Int32 _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.m_varName, _name, 32);
		shaderVariable.m_varType = SHADER_VAR_TYPE_INT;
		shaderVariable.int_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::SetShaderVec3Var(const char* _name, const Vector3& _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.m_varName, _name, 32);
		shaderVariable.m_varType = SHADER_VAR_TYPE_VECTOR3;
		shaderVariable.vec3_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::SetShaderMatVar(const char* _name, const Matrix4x4& _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.m_varName, _name, 32);
		shaderVariable.m_varType = SHADER_VAR_TYPE_MATRIX;
		shaderVariable.mat_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::SetData(const char* _name, void* _data)
	{

	}

	void ShaderAction::SetShaderTextureVar(const char* _name, GPUTexture* _texture, Int32 _texture_index)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.m_varName, _name, 32);
		shaderVariable.m_varType = SHADER_VAR_TYPE_TEXTURE;
		shaderVariable.texture_value = { _texture, _texture_index };
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::SetType(ZE::UInt16 _shaderActionType)
	{
		m_shaderActionType = _shaderActionType;
	}

	ShaderVariable::ShaderVariable(const ShaderVariable& _other)
	{
		StringFunc::WriteTo(m_varName, _other.m_varName, 32);
		m_varType = _other.m_varType;
		memcpy(&mat_value, &_other.mat_value, sizeof(mat_value));
	}

}
