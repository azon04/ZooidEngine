#ifndef __Z_SHADER_ACTION__
#define __Z_SHADER_ACTION__

#include "Shader.h"
#include "GPUBufferData.h"

namespace ZE {
class ShaderAction {
public:

	ShaderAction();
	ShaderAction(Shader* shader);

private:

	Shader* m_shader;
	GPUBufferData* m_buffers[2];
};
};
#endif // __Z_SHADER_ACTION__
