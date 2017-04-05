#ifndef __Z_SHADER__
#define __Z_SHADER__

namespace ZE {

class Shader {
	
	Shader() : m_GLProgram(0) {}
	virtual ~Shader();
	
	void LoadShaderFiles(const char* _vertexShaderFile, const char* _colorShaderFile, const char* _computeShaderFile);

	void Bind();
	void Unbind();

	// #OPENGL Specific
	unsigned int m_GLProgram;
};

};
#endif // !__Z_SHADER__

