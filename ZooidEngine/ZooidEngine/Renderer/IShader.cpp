#include "IShader.h"

#include "../ZEngine.h"
#include "FileSystem/FileReader.h"

#if Z_RENDER_OPENGL
#include <GL/glew.h>
#endif

#include <string>
#include <fstream>
#include <sstream>

namespace ZE {

	IShaderChain::~IShaderChain()
	{
		Release();
	}

	void IShaderChain::MakeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader)
	{
		
	}

	void IShaderChain::Release()
	{
	}

	void IShaderChain::SetLayout(BufferLayout* _layout)
	{
		m_layout = _layout;
	}

	void IShader::loadShader(const char* _shaderFilePath, UInt8 _shaderType)
	{
		FileReader file(_shaderFilePath);
		Handle bufferHandle(sizeof(char) * (file.size() + 1));
		Int32 resultSize = file.readToBuffer(bufferHandle.getObject(), file.size());
		char* shaderText = bufferHandle.getObject<char>();
		shaderText[resultSize] = '\0';

		m_shaderType = _shaderType;
		
		loadShaderFromBuffer(shaderText, resultSize);

		bufferHandle.release();

	}

	void IShader::release()
	{

	}

}