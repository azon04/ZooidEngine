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
}

#endif // !__ZE_SKELETON_H__
