#ifndef __ZE_MESH_H__
#define __ZE_MESH_H__

#include "ZEngineIncl.h"

namespace ZE
{
	class IGPUBufferArray;
	class IGPUBufferData;
	class GameContext;
	class Material;

	class Mesh : public Object
	{
		DEFINE_CLASS(Mesh)

	public:

		Mesh() {}

		IGPUBufferData* m_vertexBuffer;;
		IGPUBufferData* m_indexBuffer;
		IGPUBufferArray* m_bufferArray;
		Material* m_material;
	};
}
#endif
