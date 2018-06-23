#include "AnimationSM.h"

#include "Events/Events.h"

#include "Scene/RenderComponent.h"

#include "Animation/Skeleton.h"
#include "FileSystem/FileReader.h"
#include "FileSystem/DirectoryHelper.h"
#include "ResourceManagers/AnimationManager.h"
#include "Utils/Array.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(AnimationSM, Component);

	AnimationSM::AnimationSM(GameContext* _gameContext)
		: Component(_gameContext), m_currentTime(0.0f), m_stateCache(nullptr)
	{

	}

	void AnimationSM::setupComponent()
	{
		Component::setupComponent();
		addEventDelegate(Event_UPDATE, &AnimationSM::handleEventUpdate);
	}


	void AnimationSM::handleEventUpdate(Event* evt)
	{
		Event_UPDATE* pRealEvt = (Event_UPDATE*)evt;

		m_currentTime += pRealEvt->m_deltaTime / 1000.0f;

		if (m_stateCache)
		{
			if (m_parents.length() > 0)
			{
				RenderComponent* pRenderComp = (RenderComponent*)m_parents[0];
				SkeletonState* pSkelState = pRenderComp->m_hSkeletonState.getObject<SkeletonState>();

				// TODO Update Animation State
				AnimationPose pose;
				processAnimNode(pose, m_stateCache->Node, m_currentTime, pSkelState->getSkeleton());

				ZASSERT(pSkelState->getSkeleton()->getJointCount() == pose.jointPoses.length(), "Pose and Skeleton Definition mismatch!");
				
				Array<Matrix4x4> mats(pSkelState->getSkeleton()->getJointCount());

				for (int i = 0; i < pSkelState->getSkeleton()->getJointCount(); i++)
				{
					mats.push_back(Matrix4x4());
					pose.jointPoses[i].toMatrix(mats[i]);
				}

				pSkelState->setJointStateMatrices(mats, true);
			}
		}
	}

	void AnimationSM::readAnimStateDef(const char* file)
	{
		FileReader reader(file);

		if (!reader.isValid())
		{
			ZEERROR("Couldn't find Animation State Definition file : %s", file);
			return;
		}

		char buffer[256];

		// Read State Default
		reader.readNextString(buffer);
		reader.readNextString(buffer);
		m_state = buffer;

		// Read States
		
		// Read State
		reader.readNextString(buffer);
		while (!reader.eof() && StringFunc::Compare(buffer, "State") == 0)
		{
			// State Name
			reader.readNextString(buffer);

			AnimationState animState;
			animState.StateName = buffer;
			
			// Read Nodes
			reader.readNextString(buffer);

			NodeType type = getNodeTypeOnString(buffer);
			if (type != NODE_NONE)
			{
				animState.Node = readAnimNode(type, &reader);
			}

			m_stateMap.put(animState.StateName.c_str(), animState);

			// Read State
			reader.readNextString(buffer);
		} 

		m_stateCache = &m_stateMap[m_state.c_str()];

		reader.close();
	}

	ZE::AnimationNode AnimationSM::readAnimNode(NodeType type, FileReader* reader)
	{
		AnimationNode node;
		node.Type = type;

		if (type != NODE_CLIP)
		{
			node.Nodes.reset(2);
		}

		char buffer[256];

		// Read opening '{'
		reader->readNextString(buffer);

		reader->readNextString(buffer);

		while (StringFunc::Compare(buffer, "}") != 0)
		{
			if (StringFunc::Compare("Clip", buffer) == 0)
			{
				reader->readNextString(buffer);
				Handle hClip = AnimationManager::getInstance()->loadResource(GetResourcePath(buffer).c_str());
				if (hClip.isValid())
				{
					node.Clip = hClip.getObject<AnimationClip>();
				}
				else
				{
					node.Clip = nullptr;
				}
			}
			else if(StringFunc::Compare("Alpha", buffer) == 0)
			{
				node.Alpha = reader->readNextFloat();
			}
			else if (StringFunc::Compare("Duration", buffer) == 0)
			{
				node.SyncDuration = reader->readNextFloat();
			}
			else if (StringFunc::Compare("Joint", buffer) == 0)
			{
				// Read Joint
				reader->readNextString(buffer);
				node.PartialJointName = buffer;
			}
			else
			{
				// Check if it's another node
				NodeType type = getNodeTypeOnString(buffer);

				if (type != NODE_NONE)
				{
					AnimationNode childNode = readAnimNode(type, reader);
					node.Nodes.push_back(childNode);
				}
			}

			reader->readNextString(buffer);
		}

		return node;
	}

	ZE::NodeType AnimationSM::getNodeTypeOnString(const char* name)
	{
		if (StringFunc::Compare("Node_Lerp", name) == 0)
		{
			return NODE_LERP;
		}
		else if (StringFunc::Compare("Node_Clip", name) == 0)
		{
			return NODE_CLIP;
		}
		else if (StringFunc::Compare("Node_Additive", name) == 0)
		{
			return NODE_ADDITIVE;
		}
		else if (StringFunc::Compare("Node_Partial", name) == 0)
		{
			return NODE_PARTIAL;
		}

		return NODE_NONE;
	}

	void AnimationSM::processAnimNode(AnimationPose& resPose, AnimationNode& node, Float32 localTime, Skeleton* skeletonDef)
	{
		if (node.Type == NODE_CLIP)
		{
			if (node.SyncDuration > 0.0f)
			{
				node.Clip->getAnimationPoseAtTimeWithDuration(localTime, resPose, node.SyncDuration);
			}
			else
			{
				node.Clip->getAnimationPoseAtTime(localTime, resPose);
			}
		}
		else if (node.Type == NODE_LERP)
		{
			// TODO this can use a bit of threading
			AnimationPose pose1;
			AnimationPose pose2;
			processAnimNode(pose1, node.Nodes[0], localTime, skeletonDef);
			processAnimNode(pose2, node.Nodes[1], localTime, skeletonDef);

			AnimationHelper::LerpAnimPose(resPose, pose1, pose2, node.Alpha);
		}
		else if (node.Type == NODE_ADDITIVE)
		{
			// TODO this can use a bit of threading
			AnimationPose pose1;
			AnimationPose pose2;
			processAnimNode(pose1, node.Nodes[0], localTime, skeletonDef);
			processAnimNode(pose2, node.Nodes[1], localTime, skeletonDef);

			AnimationHelper::LerpAdditivePose(resPose, pose1, pose2, node.Alpha);
		}
		else if (node.Type == NODE_PARTIAL)
		{
			if (node.PartialJointIndex == -1)
			{
				skeletonDef->getJointByName(node.PartialJointName, node.PartialJointIndex);
			}
			
			// TODO this can use a bit of threading
			AnimationPose pose1;
			AnimationPose pose2;
			processAnimNode(pose1, node.Nodes[0], localTime, skeletonDef);
			processAnimNode(pose2, node.Nodes[1], localTime, skeletonDef);

			AnimationHelper::LerpAnimPartialPose(resPose, pose1, pose2, skeletonDef, node.PartialJointIndex, node.Alpha);
		}
	}


}