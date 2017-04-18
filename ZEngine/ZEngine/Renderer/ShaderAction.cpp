#include "ShaderAction.h"

namespace ZE {

	ShaderAction::ShaderAction()
	{

	}

	ShaderAction::ShaderAction(Shader* shader)
	{
		m_shader = shader;
	}

	void ShaderAction::SetShaderAndBuffer(Shader* _shader, GPUBufferArray* _bufferArray)
	{
		m_shader = _shader;
		m_bufferArray = _bufferArray;
	}

}
