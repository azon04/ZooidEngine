#include "AnimationManager.h"

#include "Animation/Animation.h"
#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"
#include "SkeletonManager.h"

namespace ZE
{

	IMPLEMENT_CLASS_1(AnimationManager, ResourceManager)

	void AnimationManager::Init(GameContext* gameContext)
	{
		Handle hAnimationManager("Animation Manager", sizeof(AnimationManager));
		s_instance = new(hAnimationManager) AnimationManager;
	}

	void AnimationManager::Destroy()
	{
		if (s_instance)
		{
			s_instance->unloadResources();
		}
	}

	Handle AnimationManager::loadResource_Internal(const char* resourceFilePath)
	{
		Handle handle;
		FileReader fileReader;
		fileReader.open(resourceFilePath);

		if (!fileReader.isValid())
		{
			ZEERROR("Animation file \"%s\" is not exist.", resourceFilePath);
			return handle;
		}

		char buffer[256];

		handle = Handle("AnimationClip", sizeof(AnimationClip));
		AnimationClip* pAnimClip = new(handle) AnimationClip();

		// Read Animation Clip and Name
		fileReader.readNextString(buffer);
		fileReader.readNextString(buffer);

		// Read duration
		fileReader.readNextString(buffer);
		pAnimClip->m_duration = fileReader.readNextFloat();

		// Read FPS
		fileReader.readNextString(buffer);
		pAnimClip->m_framePerSecond = fileReader.readNextInt();

		// Read Skeleton
		fileReader.readNextString(buffer);
		fileReader.readNextString(buffer);
		Handle hSkeleton = SkeletonManager::getInstance()->loadResource(GetResourcePath(buffer).c_str());
		if (hSkeleton.isValid())
		{
			pAnimClip->m_skeleton = hSkeleton.getObject<Skeleton>();
		}

		// Read Animation Data
		fileReader.readNextString(buffer);
		while (StringFunc::Compare(buffer, "time") == 0)
		{
			fileReader.readNextString(buffer);
			pAnimClip->m_animationSamples.push_back(AnimationPose());
			AnimationPose& animPose = pAnimClip->m_animationSamples[pAnimClip->m_animationSamples.size() - 1];
			
			while (true)
			{
				if (StringFunc::Compare(buffer, "bone") == 0)
				{
					Int32 boneIndex = fileReader.readNextInt();
					PoseSQT poseSQT;
					fileReader.readNextString(buffer);
					while (true)
					{
						if (StringFunc::Compare("T", buffer) == 0)
						{
							poseSQT.translation.setX(fileReader.readNextFloat());
							poseSQT.translation.setY(fileReader.readNextFloat());
							poseSQT.translation.setZ(fileReader.readNextFloat());
						}
						else if (StringFunc::Compare("Q", buffer) == 0)
						{
							poseSQT.rotation.m_x = fileReader.readNextFloat();
							poseSQT.rotation.m_y = fileReader.readNextFloat();
							poseSQT.rotation.m_z = fileReader.readNextFloat();
							poseSQT.rotation.m_w = fileReader.readNextFloat();
						}
						else if (StringFunc::Compare("Q3", buffer) == 0)
						{
							poseSQT.rotation.m_x = fileReader.readNextFloat();
							poseSQT.rotation.m_y = fileReader.readNextFloat();
							poseSQT.rotation.m_z = fileReader.readNextFloat();
							poseSQT.rotation.m_w = 1 - (poseSQT.rotation.m_x * poseSQT.rotation.m_x + poseSQT.rotation.m_y * poseSQT.rotation.m_y + poseSQT.rotation.m_z * poseSQT.rotation.m_z);
						}
						else if (StringFunc::Compare("S", buffer) == 0)
						{
							poseSQT.scale.setX(fileReader.readNextFloat());
							poseSQT.scale.setY(fileReader.readNextFloat());
							poseSQT.scale.setZ(fileReader.readNextFloat());
						}
						else
						{
							break;
						}
					}
					animPose.jointPoses.push_back(poseSQT);
				}
				else
				{
					break;
				}
			}
		}

		fileReader.close();

		return handle;

	}

	void AnimationManager::preUnloadResource(Resource* _resource)
	{

	}

	ZE::AnimationManager* AnimationManager::s_instance;

}