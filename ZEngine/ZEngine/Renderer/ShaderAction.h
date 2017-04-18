#ifndef __Z_SHADER_ACTION__
#define __Z_SHADER_ACTION__

#include "Shader.h"
#include "GPUBufferArray.h"

namespace ZE {
class ShaderAction {
public:

	ShaderAction();
	ShaderAction(Shader* shader);

	void SetShaderAndBuffer(Shader* _shader, GPUBufferArray* _bufferArray);

	int m_vertexSize;

	Shader* m_shader;
	GPUBufferArray* m_bufferArray;
};
};
#endif // __Z_SHADER_ACTION__
