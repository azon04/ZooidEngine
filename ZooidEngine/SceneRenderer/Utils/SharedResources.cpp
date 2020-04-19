#include "SharedResources.h"

#include "Utils/PrimitiveTypes.h"
#include "Renderer/BufferData.h"
#include "ResourceManagers/BufferManager.h"

namespace ZE
{

	ZE::IGPUBufferArray* UtilShaderResources::GetCubeVerticesOnly()
	{
		static IGPUBufferArray* cubeBufferArray = nullptr;
		if (!cubeBufferArray)
		{
			Float32 vertices[] = {
				// back face
				-1.0f, -1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f,  1.0f, -1.0f,
				// front face
				-1.0f, -1.0f,  1.0f,
				1.0f, -1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,
				// left face
				-1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f, -1.0f,
				-1.0f, -1.0f,  1.0f,
				-1.0f,  1.0f,  1.0f,
				// right face
				1.0f,  1.0f,  1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f,  1.0f,  1.0f,
				1.0f, -1.0f,  1.0f,
				// bottom face
				-1.0f, -1.0f, -1.0f,
				1.0f, -1.0f, -1.0f,
				1.0f, -1.0f,  1.0f,
				1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f,  1.0f,
				-1.0f, -1.0f, -1.0f,
				// top face
				-1.0f,  1.0f, -1.0f,
				1.0f,  1.0f , 1.0f,
				1.0f,  1.0f, -1.0f,
				1.0f,  1.0f,  1.0f,
				-1.0f,  1.0f, -1.0f,
				-1.0f,  1.0f,  1.0f
			};

			BufferData bufferData(VERTEX_BUFFER);
			bufferData.SetData(vertices, sizeof(Float32) * 3, 36);
			bufferData.setBufferLayout(BUFFER_LAYOUT_V3);
			bufferData.setStaticBuffer(true);
			cubeBufferArray = BufferManager::getInstance()->createBufferArray(&bufferData, nullptr, nullptr).getObject<IGPUBufferArray>();
		}

		return cubeBufferArray;
	}

}