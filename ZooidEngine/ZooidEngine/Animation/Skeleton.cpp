#include "Skeleton.h"
#include "FileSystem/FileReader.h"

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

	bool Skeleton::getJointByName(String name, Int32& _jointId)
	{
		for (Int32 i = 0; i < m_jointCount; i++)
		{
			if (name == m_joints[i].name)
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
	}

	void Skeleton::readJoint(FileReader* fileReader, Int32 parentJointIndex)
	{
		char buffer[128];
		SkeletonJoint joint;

		// Read Name
		fileReader->readNextString(buffer);
		joint.name = buffer;
		joint.parentIndex = parentJointIndex;

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
			else if (StringFunc::Compare(buffer, "Transform") == 0)
			{
				Matrix4x4 mat;
				for (int i = 0; i < 4; i++)
				{
					for (int j = 0; j < 4; j++)
					{
						mat.m_data[i][j] = fileReader->readNextFloat();
					}
				}

				if (parentJointIndex != -1)
				{
					m_joints[index].bindPose = m_joints[parentJointIndex].bindPose * mat;
				}
				else
				{
					m_joints[index].bindPose = mat;
				}

				m_joints[index].invBindPose = m_joints[index].bindPose.inverse();
			}

			fileReader->readNextString(buffer);
		}
	}

}