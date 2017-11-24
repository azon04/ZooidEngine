#ifndef __Z_SHADER__
#define __Z_SHADER__

#include "BufferLayout.h"

// Inner engine includes
#include "../Utils/Array.h"
#include "../Math/Vector3.h"
#include "../Math/Matrix4x4.h"

// Outer engine includes
#if Z_RENDER_OPENGL
#include <GL/glew.h>
#endif 

#define Z_SHADER_SIMPLE 0
#define Z_SHADER_3D_DEFAULT 1

namespace ZE {

	class GPUTexture;

class Shader {
public:
	Shader()
	{
#if Z_RENDER_OPENGL
		m_GLProgram = 0;
#endif
	}
	virtual ~Shader();
	
	void LoadShaderFiles(const char* _vertexShaderFile, const char* _colorShaderFile, const char* _computeShaderFile);

	void SetLayout(BufferLayout* _layout);
	void Bind();
	void Unbind();

	void SetVec3(const char* _constName, Vector3 _value);
	void SetFloat(const char* _constName, float _value);
	void SetMat(const char* _constName, Matrix4x4 _value);
	void SetInt(const char* _constName, int _value);
	void SetTexture(const char* _constName, GPUTexture* _texture, Int32 _textureIndex);
		
	BufferLayout *m_layout;

#if Z_RENDER_OPENGL
	GLint getUniformPosition(const char* _varName);

	GLuint m_GLProgram;
#endif
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

