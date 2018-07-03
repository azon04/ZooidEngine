#ifndef __ZE_ANIMATION_H__
#define __ZE_ANIMATION_H__

#include "Math/Vector3.h"
#include "Math/Quaternion.h"
#include "Math/Matrix4x4.h"
#include "Utils/Array.h"
#include "Utils/HashMap.h"
#include "Utils/String.h"
#include "GameObjectModel/Object.h"

namespace ZE
{
	class Skeleton;
	class AnimationManager;

	enum InterpolationType
	{
		INTERP_LINEAR,
		INTERP_CURVE,
		INTERP_FIXED
	};

	class TrackKey
	{
		float m_time;
	};

	class TransformTrackKey : public TrackKey
	{
		Quaternion m_rot;
		Vector3 m_translation;
		Vector3 m_scale;
	};

	class FunctionTrackKey : public TrackKey
	{
		// TODO save function to track
	};

	class Track
	{
	public:
		Track() {}

		void setName(const char* textName) { m_name = textName; }
		const char* getName() const { return m_name.const_str(); }

	protected:
		String m_name;
		InterpolationType m_interpType;
	};

	class TransformTrack : public Track
	{
	public:
		
		void getTransformAt(Float32 _localTime, Matrix4x4& _matTransform);
		void addTransformKey(TransformTrack& _transformKey);
		void removeTransformKey(Int32 index);
		void sortKeys();

		Array<TransformTrackKey, true>& getKeys() { return m_trackKeys; }

	protected:
		Array<TransformTrackKey, true> m_trackKeys;
	};

	class FunctionTrack : public Track
	{

	};

	struct PoseSQT
	{
		PoseSQT() : Rotation(0.0f, 0.0f, 0.0f, 1.0f), Translation(0.0f, 0.0f, 0.0f),
				Scale(1.0f, 1.0f, 1.0f)
		{}
		~PoseSQT() {}

		void toMatrix(Matrix4x4& mat);

		Quaternion Rotation;
		Vector3 Translation;
		Vector3 Scale;
	};

	struct AnimationPose
	{
		Array<PoseSQT, true> JointPoses;
	};

	class AnimationClip : public Object
	{
		DEFINE_CLASS(AnimationClip)

		friend class AnimationManager;

	public:
		AnimationClip() {}

		// Get animation pose based on local time of animation
		void getAnimationPoseAtTime(float _localTime, AnimationPose& outPose);

		// Get animation pose based on local time of scaled animation duration
		void getAnimationPoseAtScaleTime(float _localTime, AnimationPose& outPose, float scale);

		// Get animation pose based on local time of duration
		void getAnimationPoseAtTimeWithDuration(float _localTime, AnimationPose& outPose, float _duration);
		
		FORCEINLINE Skeleton* getSkeleton() const { return m_skeleton; }

		FORCEINLINE Float32 getClipDuration() const { return m_duration; }

	protected:
		Skeleton* m_skeleton;
		Int32 m_framePerSecond;
		Int32 m_frameCount;
		Float32 m_duration;
		Array<AnimationPose, true> m_animationSamples;
	};

	class AnimationSet : public Object
	{
		DEFINE_CLASS(AnimationSet)

	public:
		AnimationSet() {}

	protected:
		Array<AnimationClip*, true> m_animationClips;
	};

	struct AnimationData : public Object
	{
		DEFINE_CLASS(AnimationData)

	public:
		AnimationData() {}

	protected:
		AnimationClip* m_animationClip;
		HashMap<String, Track*> m_trackMap;
	};

	namespace AnimationHelper
	{
		// Lerp per component of PoseSQT (Scale, Quaternion - using slerp, Translation) based on alpha
		void LerpPoseSQT(PoseSQT& res, const PoseSQT& pose1, const PoseSQT& pose2, float alpha);

		// Lerp 2 Animation Poses based on alpha
		void LerpAnimPose(AnimationPose& res, AnimationPose& pose1, AnimationPose& pose2, float alpha);
		
		// Do partial animation and lerp on target based on pose and on alpha.
		// bBlenOrientation: whether target and pose orientation blended and apply to partial animation
		void LerpAnimPartialPose(AnimationPose& res, AnimationPose& target, AnimationPose& pose, Skeleton* skelDef, Int32 boneIndex, float alpha, bool bBlendOrientation = true );
		
		// Do lerp on additive pose based on alpha
		void LerpAdditivePose(AnimationPose& res, AnimationPose& target, AnimationPose& addPose, float alpha);
	};
}
#endif
