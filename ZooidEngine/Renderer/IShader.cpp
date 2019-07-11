#include "IShader.h"

#include "FileSystem/FileReader.h"

#include <string>
#include <fstream>
#include <sstream>

namespace ZE 
{

	IShaderChain::~IShaderChain()
	{
		release();
	}

	void IShaderChain::makeChain(IShader* vsShader, IShader* psShader, IShader* gsShader, IShader* csShader)
	{
		
	}

	void IShaderChain::release()
	{
	}

	void IShaderChain::setLayout(BufferLayout* _layout)
	{
		m_layout = _layout;
	}

	void IShader::loadShader(const char* _shaderFilePath, EShaderType _shaderType)
	{
		FileReader file(_shaderFilePath);
		Handle bufferHandle(sizeof(char) * (file.size() + 1));
		Int32 resultSize = file.readToBuffer(bufferHandle.getObject(), file.size());
		char* shaderText = bufferHandle.getObject<char>();
		shaderText[resultSize] = '\0';
		file.close();

		m_shaderType = _shaderType;
		
		loadShaderFromBuffer(shaderText, resultSize);


		bufferHandle.release();

	}

	void IShader::release()
	{

	}

}