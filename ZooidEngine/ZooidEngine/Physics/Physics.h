#ifndef __PHYSICS_H__
#define __PHYSICS_H__

#include "Memory/Handle.h"
#include "Math/Vector3.h"
#include "Math/Matrix4x4.h"
#include "Utils/PrimitiveTypes.h"
#include "Utils/Array.h"

namespace ZE
{
	enum PhysicsShape : UInt16
	{
		BOX,
		SPHERE,
		CAPSULE,
		PLANE,
		CONVEX_MESHES,
		TRIANGLE_MESHES,
		HEIGHT_FIELDS,
		COMPOUND_SHAPE
	};

	struct PhysicsBodyDesc
	{
		Matrix4x4 Transform;
		bool bStatic;
		bool bGhost;
		Float32 Mass;
	};

	struct BoxBodyDesc : public PhysicsBodyDesc
	{
		Vector3 HalfExtent;
	};

	struct CapsuleBodyDesc : public PhysicsBodyDesc
	{
		Float32 Radius;
		Float32 HalfHeight;
	};

	struct ConvexMeshBodyDesc : public PhysicsBodyDesc
	{
		void* MeshData;
	};

	struct HeightFieldBodyDesc : public PhysicsBodyDesc
	{
		void* HeightFieldData;
	};

	struct PlaneBodyDesc : public PhysicsBodyDesc
	{

	};

	struct SphereBodyDesc : public PhysicsBodyDesc
	{
		Float32 Radius;
	};

	struct TriangleMeshBodyDesc : public PhysicsBodyDesc
	{
		void* TriangleMeshData;
	};

	struct CompoundBodyDesc : public PhysicsBodyDesc
	{
		Array<PhysicsBodyDesc*> m_bodyList;
	};


	class IPhysics
	{
	public:

		IPhysics() {}
		virtual ~IPhysics() {}

		virtual void Setup() = 0;
		virtual void PreUpdate() = 0;
		virtual void Update() = 0;
		virtual void PostUpdate() = 0;
		virtual void DrawDebug() = 0;
		virtual void Destroy() = 0;

		virtual Handle CreateDynamicRigidBody(PhysicsShape _shape, PhysicsBodyDesc* _data) = 0;
		virtual Handle CreateStaticRigidBody(PhysicsShape _shape, PhysicsBodyDesc* _data) = 0;

		virtual void DestroyPhysicsObject(Handle handle) = 0;
	};
}
#endif
