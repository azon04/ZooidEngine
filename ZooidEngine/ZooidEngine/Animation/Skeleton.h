#ifndef __ZE_SKELETON_H__
#define __ZE_SKELETON_H__

#include "Utils/String.h"
#include "Utils/PrimitiveTypes.h"
#include "Utils/Array.h"
#include "Math/Matrix4x4.h"
#include "GameObjectModel/Object.h"

namespace ZE
{
	class FileReader;

	struct SkeletonJoint
	{
		String name;
		Int32 parentIndex;
		Matrix4x4 invBindPose;
		Matrix4x4 bindPose;
	};

	struct FastJoint
	{
		String name;
		FastJoint* parent;
		SkeletonJoint* skeletonJoint;
		Array<FastJoint*, true> childs;
	};

	class Skeleton : public Object
	{
		DEFINE_CLASS(Skeleton)

		friend class SkeletonState;

	public:
		Skeleton() : m_uniqueId(-1), m_jointCount(0) {}
		Skeleton(Int32 _uniqueId) : m_uniqueId(_uniqueId), m_jointCount(0) {}

		FORCEINLINE Int32 getJointCount() const { return m_jointCount; }

		Int32 addJoint(SkeletonJoint& _joint);
		bool getJoint(Int32 _jointId, SkeletonJoint& _joint);
		bool getJointByName(String name, Int32& _jointId);
		bool getParentJoint(SkeletonJoint& _joint);

		void loadFromFile(const char* filePath);

	protected:
		void readJoint(FileReader* fileReader, Int32 m_parentJointIndex);

	protected:
		Int32 m_uniqueId;
		Int32 m_jointCount;
		Array<SkeletonJoint, true> m_joints;
		Array<FastJoint, true> m_fastJoints;
	};

	struct SkeletonJointState
	{
		SkeletonJointState() {}
		~SkeletonJointState() {}

		Int32 index;
		Matrix4x4 bindPose;
	};

	// Skeleton State holds the skeleton final pose to render
	// and serve as a cache, also query about bone/joint position
	class SkeletonState : public Object
	{
		DEFINE_CLASS(SkeletonState)

	public:

		SkeletonState(Skeleton* _skeleton);

		~SkeletonState() {}

		Vector3 GetJointLocation(Int32 jointIndex);
		Quaternion GetJointQuat(Int32 jointIndex);
		void getBindPoseMatrix(Int32 jointIndex, Matrix4x4& bindPose);
		void getJointMatrixPallete(Int32 jointIndex, Matrix4x4& matrixPallete);

		Skeleton* getSkeleton() const { return m_skeleton; }

	protected:
		void setupState();

	protected:
		Skeleton* m_skeleton;
		Array<SkeletonJointState> m_skeletonJointStates;
	};
}

#endif // !__ZE_SKELETON_H__
