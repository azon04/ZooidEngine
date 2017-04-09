#ifndef __Z_SHADER__
#define __Z_SHADER__

#include "BufferLayout.h"

namespace ZE {

class Shader {
	
	Shader() : m_GLProgram(0) {}
	virtual ~Shader();
	
	void LoadShaderFiles(const char* _vertexShaderFile, const char* _colorShaderFile, const char* _computeShaderFile);

	void SetLayout(BufferLayout* _layout);
	void Bind();
	void Unbind();

	BufferLayout *m_layout;
	// #OPENGL Specific
	unsigned int m_GLProgram;
};

};
#endif // !__Z_SHADER__

