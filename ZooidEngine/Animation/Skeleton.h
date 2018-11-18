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
		String Name;
		Int32 ParentIndex;
		ALIGN16 Matrix4x4 InvBindPose;
		ALIGN16 Matrix4x4 BindPose;
	};

	struct FastJoint
	{
		String Name;
		FastJoint* Parent;
		SkeletonJoint* pSkeletonJoint;
		Array<FastJoint*, true> Childs;
	};

	class Skeleton : public Object
	{
		DEFINE_CLASS(Skeleton)

		friend class SkeletonState;

	public:
		Skeleton() : m_uniqueId(-1), m_jointCount(0) {}
		Skeleton(Int32 _uniqueId) : m_uniqueId(_uniqueId), m_jointCount(0) {}

		// Add joint to the skeleton, return the index of the joint
		Int32 addJoint(SkeletonJoint& _joint);

		// Get Joint at index, return true if found
		bool getJoint(Int32 _jointId, SkeletonJoint& _joint);

		// Get index of joint with name, return true if found
		bool getJointIndexByName(const String& name, Int32& _jointId);

		// Get parent joint, return true if found
		bool getParentJoint(SkeletonJoint& _joint);

		// Get joint by name, return true if found
		bool getJointByName(const String& _jointName, SkeletonJoint& _jointOut);

		// Load joint from file
		void loadFromFile(const char* filePath);

		FORCEINLINE Int32 getJointCount() const { return m_jointCount; }
	
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

		Int32 Index;
		Matrix4x4 BindPose;
	};

	// Skeleton State holds the skeleton final pose to render
	// and serve as a cache, also query about bone/joint position
	class SkeletonState : public Object
	{
		DEFINE_CLASS(SkeletonState)

	public:

		SkeletonState(Skeleton* _skeleton);

		~SkeletonState() {}

		// Get joint location by joint index
		Vector3 GetJointLocation(Int32 jointIndex);
		
		// Get joint rotation (in Quaternion) by joint index
		Quaternion GetJointQuat(Int32 jointIndex);

		// Get Bind Pose matrix by joint index
		void getBindPoseMatrix(Int32 jointIndex, Matrix4x4& bindPose);

		// Get Matrix Pallete for joint index
		void getJointMatrixPallete(Int32 jointIndex, Matrix4x4& matrixPallete);

		// Set joint state matrices;
		// inBoneTransform: true if the matrices in bone transform
		void setJointStateMatrices(Array<Matrix4x4>& matrices, bool inBoneTransform = true);

		FORCEINLINE Skeleton* getSkeleton() const { return m_skeleton; }

	protected:
		void setupState();

	protected:
		Skeleton* m_skeleton;
		Array<SkeletonJointState> m_skeletonJointStates;
	};
}

#endif
