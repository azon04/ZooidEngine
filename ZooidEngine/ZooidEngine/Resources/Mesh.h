#ifndef __ZE_MESH_H__
#define __ZE_MESH_H__

#include "GameObjectModel/Object.h"
#include "Memory/Handle.h"

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

		Mesh() :
			m_doubleSided(false)
		{}

		bool hasSkeleton() const { return m_hSkeleton.isValid(); }

		IGPUBufferData* m_vertexBuffer;;
		IGPUBufferData* m_indexBuffer;
		IGPUBufferArray* m_bufferArray;
		Material* m_material;
		Handle m_hPhysicsBodySetup;
		Handle m_hSkeleton;

		bool m_doubleSided;
	};
}
#endif
