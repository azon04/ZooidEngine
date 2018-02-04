#include "ShaderAction.h"

#include "../Utils/StringFunc.h"

#include <cstring>

namespace ZE {

	ShaderAction::ShaderAction()
	{
		m_shaderActionType = SHADER_ACTION_DRAW;
	}

	ShaderAction::ShaderAction(IShaderChain* shader)
	{
		m_shader = shader;
	}

	void ShaderAction::Reset()
	{
		m_shader = nullptr;
		m_shaderVariables.clear();
		m_shaderFeatures.clear();
		m_bufferArray = nullptr;
		m_shaderActionType = SHADER_ACTION_DRAW;
		m_vertexSize = 0;
	}

	void ShaderAction::SetShaderAndBuffer(IShaderChain* _shader, IGPUBufferArray* _bufferArray)
	{
		m_shader = _shader;
		m_bufferArray = _bufferArray;
		m_vertexSize = _bufferArray->m_dataCount;
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

	void ShaderAction::SetShaderTextureVar(const char* _name, IGPUTexture* _texture, Int32 _texture_index)
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

	void ShaderAction::SetConstantsBlockBuffer(const char* _name, IGPUBufferData* _constantBlockBuffer)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.m_varName, _name, 32);
		shaderVariable.m_varType = SHADER_VAR_TYPE_BLOCK_BUFFER;
		shaderVariable.constant_buffer = _constantBlockBuffer;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::AddShaderFeature(UInt32 _feature, bool _enabled)
	{
		ShaderFeature shaderFeature;
		shaderFeature.m_rendererFeature = _feature;
		shaderFeature.m_bFeatureEnabled = _enabled;
		m_shaderFeatures.push_back(shaderFeature);
	}

	ShaderVariable::ShaderVariable(const ShaderVariable& _other)
	{
		StringFunc::WriteTo(m_varName, _other.m_varName, 32);
		m_varType = _other.m_varType;
		memcpy(&mat_value, &_other.mat_value, sizeof(mat_value));
	}

	void EnableAndSetDepthFunction(ShaderAction& shaderAction, RendererCompareFunc func)
	{
		shaderAction.m_shaderFeatures.push_back(ShaderFeature());
		ShaderFeature& shaderFeature = shaderAction.m_shaderFeatures[shaderAction.m_shaderFeatures.length() - 1];
		shaderFeature.m_rendererFeature = DEPTH_TEST;
		shaderFeature.m_bFeatureEnabled = true;
		
		ShaderFeatureVar shaderFunc;
		shaderFunc.uint_value = func;
		
		shaderFeature.m_shaderFeatureVar.push_back(shaderFunc);
	}

	void EnableAndSetStencilFunc(ShaderAction& shaderAction, RendererCompareFunc func, Int32 ref, UInt32 refMask, UInt32 stencilWriteMask)
	{
		shaderAction.m_shaderFeatures.push_back(ShaderFeature());
		ShaderFeature& shaderFeature = shaderAction.m_shaderFeatures[shaderAction.m_shaderFeatures.length() - 1];
		shaderFeature.m_rendererFeature = STENCIL_TEST;
		shaderFeature.m_bFeatureEnabled = true;

		ShaderFeatureVar shaderFunc;
		shaderFunc.uint_value = func;

		shaderFeature.m_shaderFeatureVar.push_back(shaderFunc);

		ShaderFeatureVar refVar;
		refVar.int_value = ref;

		shaderFeature.m_shaderFeatureVar.push_back(refVar);

		ShaderFeatureVar refMaskVar;
		refMaskVar.uint_value = refMask;

		shaderFeature.m_shaderFeatureVar.push_back(refMaskVar);
		
		ShaderFeatureVar stencilMaskVar;
		stencilMaskVar.uint_value = stencilWriteMask;

		shaderFeature.m_shaderFeatureVar.push_back(stencilMaskVar);

	}

}
