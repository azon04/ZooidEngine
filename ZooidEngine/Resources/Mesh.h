#ifndef __ZE_MESH_H__
#define __ZE_MESH_H__

#include "GameObjectModel/Object.h"
#include "Renderer/IGPUBufferArray.h"
#include "Memory/Handle.h"
#include "Math/Shapes.h"

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
			m_material(nullptr),
			m_doubleSided(false)
		{}

		FORCEINLINE bool hasSkeleton() const { return m_hSkeleton.isValid(); }
		FORCEINLINE bool isDoubleSided() const { return m_doubleSided; }

		FORCEINLINE IGPUBufferArray* getGPUBufferArray() const { return m_bufferArray; }
		FORCEINLINE Handle getSkeletonHandle() { return m_hSkeleton; }
		FORCEINLINE Material* getMaterial() { return m_material; }
		FORCEINLINE Handle getPhysicsBodySetup() { return m_hPhysicsBodySetup; }
		FORCEINLINE ERenderTopologyEnum getRenderTopology() const { return (m_bufferArray ? m_bufferArray->getRenderTopology() : ERenderTopologyEnum::TOPOLOGY_TRIANGLE); }

		Sphere getBoundingSphere(const Matrix4x4& transform);
		AxisAlignedBox getAABBoundingBox(const Matrix4x4& transform);
		OrientedBox getOBBoundingBox(const Matrix4x4& transform);

		Vector3 getLocalBoxMin() { return m_boxMin; }
		Vector3 getLocalBoxMax() { return m_boxMax; }
		Vector3 getLocalBoxCenter() { return (m_boxMax + m_boxMin) * 0.5f; }
		Vector3 getLocalBoxExtend() { return (m_boxMax - m_boxMin) * 0.5f; }
		Vector3 getLocalCenterOffset() { return m_centerOffset; }

	protected:
		IGPUBufferData* m_vertexBuffer;
		IGPUBufferData* m_indexBuffer;
		IGPUBufferArray* m_bufferArray;
		Material* m_material;
		Handle m_hPhysicsBodySetup;
		Handle m_hSkeleton;
		
		// For Culling
		Float32 m_radius;
		Vector3 m_boxMin;
		Vector3 m_boxMax;
		Vector3 m_centerOffset;

		bool m_doubleSided;
	};
}
#endif
