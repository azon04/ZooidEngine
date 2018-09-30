#include "ShaderAction.h"

#include "Utils/StringFunc.h"

#include <cstring>

namespace ZE 
{

	ShaderAction::ShaderAction()
	{
		m_shaderActionType = SHADER_ACTION_DRAW;
	}

	ShaderAction::ShaderAction(IShaderChain* shader)
	{
		m_shader = shader;
	}

	void ShaderAction::reset()
	{
		m_shader = nullptr;
		m_shaderVariables.clear();
		m_shaderFeatures.clear();
		m_bufferArray = nullptr;
		m_shaderActionType = SHADER_ACTION_DRAW;
		m_vertexSize = 0;
		m_bSkin = false;
	}

	void ShaderAction::setShaderChain(IShaderChain* _shader)
	{
		m_shader = _shader;
	}

	void ShaderAction::setBuffer(IGPUBufferArray* _bufferArray)
	{
		m_bufferArray = _bufferArray;
		m_vertexSize = _bufferArray->getDataCount();
	}

	void ShaderAction::setShaderAndBuffer(IShaderChain* _shader, IGPUBufferArray* _bufferArray)
	{
		m_shader = _shader;
		m_bufferArray = _bufferArray;
		m_vertexSize = _bufferArray->getDataCount();
	}

	void ShaderAction::setShaderFloatVar(const char* _name, float _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.VarName, _name, 32);
		shaderVariable.VarType = SHADER_VAR_TYPE_FLOAT;
		shaderVariable.Float_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::setShaderIntVar(const char* _name, Int32 _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.VarName, _name, 32);
		shaderVariable.VarType = SHADER_VAR_TYPE_INT;
		shaderVariable.Int_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::setShaderVec3Var(const char* _name, const Vector3& _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.VarName, _name, 32);
		shaderVariable.VarType = SHADER_VAR_TYPE_VECTOR3;
		shaderVariable.Vec3_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::setShaderMatVar(const char* _name, const Matrix4x4& _value)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.VarName, _name, 32);
		shaderVariable.VarType = SHADER_VAR_TYPE_MATRIX;
		shaderVariable.Mat_value = _value;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::setData(const char* _name, void* _data)
	{

	}

	void ShaderAction::setShaderTextureVar(const char* _name, IGPUTexture* _texture, Int32 _texture_index)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.VarName, _name, 32);
		shaderVariable.VarType = SHADER_VAR_TYPE_TEXTURE;
		shaderVariable.texture_value = { _texture, _texture_index };
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::setType(ZE::UInt16 _shaderActionType)
	{
		m_shaderActionType = _shaderActionType;
	}

	void ShaderAction::setConstantsBlockBuffer(const char* _name, IGPUBufferData* _constantBlockBuffer)
	{
		ShaderVariable shaderVariable;
		StringFunc::WriteTo(shaderVariable.VarName, _name, 32);
		shaderVariable.VarType = SHADER_VAR_TYPE_BLOCK_BUFFER;
		shaderVariable.Constant_buffer = _constantBlockBuffer;
		m_shaderVariables.push_back(shaderVariable);
	}

	void ShaderAction::getShaderMatVar(const char* _name, Matrix4x4& _value)
	{
		for (int i = 0; i < m_shaderVariables.length(); i++)
		{
			if (StringFunc::Compare(m_shaderVariables[i].VarName, _name) == 0)
			{
				_value = m_shaderVariables[i].Mat_value;
				return;
			}
		}
	}

	void ShaderAction::addShaderFeature(UInt32 _feature, bool _enabled)
	{
		ShaderFeature shaderFeature;
		shaderFeature.m_rendererFeature = _feature;
		shaderFeature.m_bFeatureEnabled = _enabled;
		m_shaderFeatures.push_back(shaderFeature);
	}

	ShaderVariable::ShaderVariable(const ShaderVariable& _other)
	{
		StringFunc::WriteTo(VarName, _other.VarName, 32);
		VarType = _other.VarType;
		memcpy(&Mat_value, &_other.Mat_value, sizeof(Mat_value));
	}

	void EnableAndSetDepthFunction(ShaderAction& shaderAction, ERendererCompareFunc func)
	{
		shaderAction.m_shaderFeatures.push_back(ShaderFeature());
		ShaderFeature& shaderFeature = shaderAction.m_shaderFeatures[shaderAction.m_shaderFeatures.length() - 1];
		shaderFeature.m_rendererFeature = DEPTH_TEST;
		shaderFeature.m_bFeatureEnabled = true;
		
		ShaderFeatureVar shaderFunc;
		shaderFunc.Uint_value = func;
		
		shaderFeature.m_shaderFeatureVar.push_back(shaderFunc);
	}

	void EnableAndSetStencilFunc(ShaderAction& shaderAction, ERendererCompareFunc func, Int32 ref, UInt32 refMask, UInt32 stencilWriteMask)
	{
		shaderAction.m_shaderFeatures.push_back(ShaderFeature());
		ShaderFeature& shaderFeature = shaderAction.m_shaderFeatures[shaderAction.m_shaderFeatures.length() - 1];
		shaderFeature.m_rendererFeature = STENCIL_TEST;
		shaderFeature.m_bFeatureEnabled = true;

		ShaderFeatureVar shaderFunc;
		shaderFunc.Uint_value = func;

		shaderFeature.m_shaderFeatureVar.push_back(shaderFunc);

		ShaderFeatureVar refVar;
		refVar.Int_value = ref;

		shaderFeature.m_shaderFeatureVar.push_back(refVar);

		ShaderFeatureVar refMaskVar;
		refMaskVar.Uint_value = refMask;

		shaderFeature.m_shaderFeatureVar.push_back(refMaskVar);
		
		ShaderFeatureVar stencilMaskVar;
		stencilMaskVar.Uint_value = stencilWriteMask;

		shaderFeature.m_shaderFeatureVar.push_back(stencilMaskVar);

	}

	void EnableAndSetBlendFunc(ShaderAction& shaderAction, ERendererBlendFactor sourceBlendFactor, ERendererBlendFactor dstBlendFactor)
	{
		shaderAction.m_shaderFeatures.push_back(ShaderFeature());
		ShaderFeature& shaderFeature = shaderAction.m_shaderFeatures[shaderAction.m_shaderFeatures.length() - 1];
		shaderFeature.m_rendererFeature = BLEND;
		shaderFeature.m_bFeatureEnabled = true;

		ShaderFeatureVar sourceBlendFactorVar;
		sourceBlendFactorVar.Uint_value = sourceBlendFactor;

		shaderFeature.m_shaderFeatureVar.push_back(sourceBlendFactorVar);

		ShaderFeatureVar destBlendFactorVar;
		destBlendFactorVar.Uint_value = dstBlendFactor;

		shaderFeature.m_shaderFeatureVar.push_back(destBlendFactorVar);
	}

	void EnableAndSetFaceCull(ShaderAction& shaderAction, EFaceFrontOrder faceOrder, ECullFace cullFace)
	{
		shaderAction.m_shaderFeatures.push_back(ShaderFeature());
		ShaderFeature& shaderFeature = shaderAction.m_shaderFeatures[shaderAction.m_shaderFeatures.length() - 1];
		shaderFeature.m_rendererFeature = FACE_CULING;
		shaderFeature.m_bFeatureEnabled = true;

		ShaderFeatureVar faceOrderVar;
		faceOrderVar.Uint_value = faceOrder;

		shaderFeature.m_shaderFeatureVar.push_back(faceOrderVar);

		ShaderFeatureVar cullFaceVar;
		cullFaceVar.Uint_value = cullFace;

		shaderFeature.m_shaderFeatureVar.push_back(cullFaceVar);
	}

}
