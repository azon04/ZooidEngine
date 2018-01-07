#ifndef __ZE_MESH_H__
#define __ZE_MESH_H__

#include "ZEngineIncl.h"

namespace ZE
{
	class GPUBufferArray;
	class GPUBufferData;
	class GameContext;

	class Mesh : public Object
	{
		DEFINE_CLASS(Mesh)

	public:

		Mesh() {}

		GPUBufferData* m_vertexBuffer;;
		GPUBufferData* m_indexBuffer;
		GPUBufferArray* m_bufferArray;
	};
}
#endif
