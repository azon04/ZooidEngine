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
		PoseSQT() : rotation(0.0f, 0.0f, 0.0f, 1.0f), translation(0.0f, 0.0f, 0.0f),
				scale(1.0f, 1.0f, 1.0f)
		{}
		~PoseSQT() {}

		Quaternion rotation;
		Vector3 translation;
		Vector3 scale;
	};

	struct AnimationPose
	{
		Array<PoseSQT, true> jointPoses;
	};

	class AnimationClip : public Object
	{
		DEFINE_CLASS(AnimationClip)

	public:
		AnimationClip() {}

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

		Array<AnimationClip*, true> m_animationClips;
	};

	struct AnimationData : public Object
	{
		DEFINE_CLASS(AnimationData)

	public:
		AnimationData() {}

		AnimationClip* m_animationClip;
		HashMap<String, Track*> m_trackMap;
	};
}
#endif
