#ifndef __ZE_ANIMATION_SM_H__
#define __ZE_ANIMATION_SM_H__

#include "GameObjectModel/Component.h"

#include "Animation/Animation.h"

namespace ZE
{
	class GameContext;
	class AnimationClip;
	class FileReader;

	enum NodeType : UInt8
	{
		NODE_NONE,
		NODE_CLIP,
		NODE_LERP,
		NODE_ADDITIVE,
		NODE_PARTIAL
	};

	struct AnimationNode
	{
		AnimationNode() {}
		~AnimationNode() {}

		NodeType Type;

		union 
		{
			AnimationClip* Clip;
			Float32 Alpha;
		};

		Array<AnimationNode> Nodes;
		Int32 PartialJointIndex = -1;
		String PartialJointName;
		String VariableName;
		Float32 SyncDuration = -1;
	};

	struct AnimationState
	{
		String StateName;
		AnimationNode Node;
	};

	class AnimationSM : public Component
	{
		DEFINE_CLASS(AnimationSM);

	public:

		AnimationSM(GameContext* _gameContext);

		virtual ~AnimationSM() {}

		virtual void setupComponent();

		// Handle update event
		virtual void handleEventUpdate(Event* evt);

		// Read Anim State Definition file
		void readAnimStateDef(const char* file);

	protected:

		void processAnimNode(AnimationPose& resPos, AnimationNode& node, Float32 localTime, Skeleton* skeletonDef);
		AnimationNode readAnimNode(NodeType type, FileReader* reader);
		NodeType getNodeTypeOnString(const char* name);

	protected:

		String m_state;
		HashMap<String, AnimationState> m_stateMap;
		Float32 m_currentTime;

		AnimationState* m_stateCache;

	};
}
#endif
