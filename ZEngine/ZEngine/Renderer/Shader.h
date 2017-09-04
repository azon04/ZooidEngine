#ifndef __Z_SHADER__
#define __Z_SHADER__

#include "BufferLayout.h"

#include "../Utils/Array.h"

// OPENGL Specific
#include <GL/glew.h>

namespace ZE {

class Shader {
public:
	Shader() : m_GLProgram(0) {}
	virtual ~Shader();
	
	void LoadShaderFiles(const char* _vertexShaderFile, const char* _colorShaderFile, const char* _computeShaderFile);

	void SetLayout(BufferLayout* _layout);
	void Bind();
	void Unbind();

	BufferLayout *m_layout;

	// #OPENGL Specific
	GLuint m_GLProgram;
};

class ShaderManager {
	
	static ShaderManager* m_instance;

	ShaderManager();

public:
	
	static void Init();
	static void Destroy();
	static ShaderManager* getInstance();

	void InitShaders();
	void DestroyShaders();

	Array<Shader*> m_shaders;

};

};
#endif // !__Z_SHADER__

