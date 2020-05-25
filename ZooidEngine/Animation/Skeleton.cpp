#include "Skeleton.h"
#include "FileSystem/FileReader.h"

#include "ResourceManagers/BufferManager.h"

#include "Renderer/IRenderer.h"
#include "ZEGameContext.h"

namespace ZE
{
	IMPLEMENT_CLASS_0(Skeleton)

	ZE::Int32 Skeleton::addJoint(SkeletonJoint& _joint)
	{
		m_joints.push_back(_joint);
		m_jointCount++;
		return m_jointCount - 1;
	}

	bool Skeleton::getJoint(Int32 _jointId, SkeletonJoint& _joint)
	{
		if (_jointId >= m_jointCount)
		{
			return false;
		}

		_joint = m_joints[_jointId];

		return true;
	}

	bool Skeleton::getJointIndexByName(const String& name, Int32& _jointId)
	{
		for (Int32 i = 0; i < m_jointCount; i++)
		{
			if (name == m_joints[i].Name)
			{
				_jointId = i;
				return true;
			}
		}
		return false;
	}

	bool Skeleton::getParentJoint(SkeletonJoint& _joint)
	{
		if (m_jointCount == 0)
		{
			return false;
		}

		_joint = m_joints[0];
		return true;
	}

	bool Skeleton::getJointByName(const String& _jointName, SkeletonJoint& _jointOut)
	{
		for (Int32 i = 0; i < m_jointCount; i++)
		{
			if (_jointName == m_joints[i].Name)
			{
				_jointOut = m_joints[i];
				return true;
			}
		}
		return false;
	}

	void Skeleton::loadFromFile(const char* filePath)
	{
		FileReader fileReader;
		fileReader.open(filePath);

		if (!fileReader.isValid())
		{
			ZEWARNING("Mesh file not found : %s", filePath);
			return;
		}

		char buffer[56];
		fileReader.readNextString(buffer);
		if (StringFunc::Compare(buffer, "Joint") == 0)
		{
			readJoint(&fileReader, -1);
		}

		fileReader.close();
	}

	void Skeleton::readJoint(FileReader* fileReader, Int32 parentJointIndex)
	{
		char buffer[128];
		SkeletonJoint joint;

		// Read Name
		fileReader->readNextString(buffer);
		joint.Name = buffer;
		joint.ParentIndex = parentJointIndex;

		Int32 index = addJoint(joint);

		// Read "{"
		fileReader->readNextString(buffer);

		// Read first token
		fileReader->readNextString(buffer);

		while (StringFunc::Compare(buffer, "}") != 0)
		{
			if (StringFunc::Compare(buffer, "Joint") == 0)
			{
				readJoint(fileReader, index);
			}
			else if (StringFunc::Compare(buffer, "InvXform") == 0)
			{
				Matrix4x4 mat;
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						mat.m_data[i][j] = fileReader->readNextFloat();
					}
				}

				m_joints[index].InvBindPose = mat;
				m_joints[index].BindPose = mat.inverse();

			}
			else if (StringFunc::Compare(buffer, "Xform") == 0)
			{
				Matrix4x4 mat;
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						mat.m_data[i][j] = fileReader->readNextFloat();
					}
				}

				m_joints[index].BindPose = mat;
				m_joints[index].InvBindPose = mat.inverse();

			}

			fileReader->readNextString(buffer);
		}
	}

	IMPLEMENT_CLASS_0(SkeletonState)

	SkeletonState::SkeletonState(Skeleton* _skeleton) : m_skeleton(_skeleton)
	{
		setupState();
	}

	Vector3 SkeletonState::GetJointLocation(Int32 jointIndex)
	{
		if (jointIndex < m_skeletonJointStates.length())
		{
			return m_skeletonJointStates[jointIndex].BindPose.getPos();
		}

		return Vector3();
	}

	Quaternion SkeletonState::GetJointQuat(Int32 jointIndex)
	{
		if (jointIndex < m_skeletonJointStates.length())
		{
			return m_skeletonJointStates[jointIndex].BindPose.toQuaternion();
		}

		return Quaternion();
	}

	void SkeletonState::getBindPoseMatrix(Int32 jointIndex, Matrix4x4& bindPose)
	{
		if (jointIndex < m_skeletonJointStates.length())
		{
			bindPose = m_skeletonJointStates[jointIndex].BindPose;
		}
	}

	void SkeletonState::getJointMatrixPallete(Int32 jointIndex, Matrix4x4& matrixPallete)
	{
		if (jointIndex < m_skeletonJointStates.length())
		{
			matrixPallete = m_skeleton->m_joints[jointIndex].InvBindPose * m_skeletonJointStates[jointIndex].BindPose;
		}
	}

	void SkeletonState::setJointStateMatrices(Array<Matrix4x4>& matrices, bool inBoneTransform)
	{
		ZASSERT(matrices.length() == m_skeletonJointStates.length(), "Matrices and Number of skeleton don't match");
		for (int i = 0; i < matrices.length(); i++)
		{
			m_skeletonJointStates[i].BindPose = matrices[i];
		}

		if (inBoneTransform)
		{
			for (int i = 0; i < matrices.length(); i++)
			{
				int parentIndex = m_skeleton->m_joints[i].ParentIndex;
				if (parentIndex >= 0)
				{
					m_skeletonJointStates[i].BindPose = m_skeletonJointStates[i].BindPose * m_skeletonJointStates[parentIndex].BindPose;
				}
			}
		}

		m_bDirty = true;
	}

	void SkeletonState::updateBuffer()
	{
		if (m_bDirty)
		{
			for (int i = 0; i < m_skeleton->getJointCount(); i++)
			{
				Matrix4x4& jointPallete = m_stateBufferData[i];
				getJointMatrixPallete(i, jointPallete);
			}

			// #TODO probably send command to render thread
			{
				ScopedRenderThreadOwnership scope(gGameContext->getRenderer());

				m_gpuStateBuffer->refresh();
			}

			m_bDirty = false;
		}
	}

	void SkeletonState::setupState()
	{
		m_skeletonJointStates.reset(m_skeleton->getJointCount());
		
		for (int i = 0; i < m_skeleton->getJointCount(); i++)
		{
			SkeletonJoint joint;
			m_skeleton->getJoint(i, joint);
			SkeletonJointState jointState;
			jointState.Index = i;
			jointState.BindPose = joint.BindPose;
			m_skeletonJointStates.push_back(jointState);
		}

		m_stateBufferData.reset(m_skeleton->getJointCount());

		m_gpuStateBuffer = BufferManager::getInstance()->createConstantBuffer(m_stateBufferData.data(), sizeof(Matrix4x4) * m_skeleton->getJointCount(), CONSTANT_BUFFER_SKELETON_DATA_INDEX);
		m_gpuStateBuffer->setDebugName("SkeletonStateBufferData");

		m_bDirty = true;
	}

}