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

		friend class MeshManager;
	public:

		Mesh() :
			m_doubleSided(false)
		{}

		FORCEINLINE bool hasSkeleton() const { return m_hSkeleton.isValid(); }
		FORCEINLINE bool isDoubleSided() const { return m_doubleSided; }

		FORCEINLINE IGPUBufferArray* getGPUBufferArray() const { return m_bufferArray; }
		FORCEINLINE Handle getSkeletonHandle() { return m_hSkeleton; }
		FORCEINLINE Material* getMaterial() { return m_material; }
		FORCEINLINE Handle getPhysicsBodySetup() { return m_hPhysicsBodySetup; }

	protected:
		IGPUBufferData* m_vertexBuffer;
		IGPUBufferData* m_indexBuffer;
		IGPUBufferArray* m_bufferArray;
		Material* m_material;
		Handle m_hPhysicsBodySetup;
		Handle m_hSkeleton;

		bool m_doubleSided;
	};
}
#endif
