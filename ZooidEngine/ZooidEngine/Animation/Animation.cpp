#include "Animation.h"

#include "Math/MathOps.h"
#include "Skeleton.h"

namespace ZE
{

	IMPLEMENT_CLASS_0(AnimationClip)

	void AnimationClip::getAnimationPoseAtTime(float _localTime, AnimationPose& outPose)
	{
		getAnimationPoseAtTimeWithDuration(_localTime, outPose, m_duration);
	}

	void AnimationClip::getAnimationPoseAtScaleTime(float _localTime, AnimationPose& outPose, float scale)
	{
		// Wrap the time by duration
		float scaledDuration = m_duration * scale;
		getAnimationPoseAtTimeWithDuration(_localTime, outPose, scaledDuration);
	}

	void AnimationClip::getAnimationPoseAtTimeWithDuration(float _localTime, AnimationPose& outPose, float _duration)
	{
		// Wrap the time by duration
		while (_localTime > _duration)
		{
			_localTime = _localTime - _duration;
		}

		// Find the current index and next index
		Int32 cIndex, nextIndex;

		float portion = _localTime / _duration;
		cIndex = portion * m_frameCount;
		nextIndex = (cIndex + 1) % m_frameCount;
		float fraction = (portion * m_frameCount) - cIndex;

		// Interpolate between Animation Poses
		AnimationPose& cPose = m_animationSamples[cIndex];
		AnimationPose& nextPose = m_animationSamples[nextIndex];
		for (int i = 0; i < cPose.JointPoses.length(); i++)
		{
			outPose.JointPoses.push_back(PoseSQT());
			PoseSQT& outPoseSQT = outPose.JointPoses[i];
			PoseSQT& cSQT = cPose.JointPoses[i];
			PoseSQT& nextSQT = nextPose.JointPoses[i];

			outPoseSQT.Translation = MathOps::VLerp(cSQT.Translation, nextSQT.Translation, fraction);
			outPoseSQT.Rotation = MathOps::QSlerp(cSQT.Rotation, nextSQT.Rotation, fraction);
			outPoseSQT.Scale = MathOps::VLerp(cSQT.Scale, nextSQT.Scale, fraction);
		}
	}

	IMPLEMENT_CLASS_0(AnimationData)
	IMPLEMENT_CLASS_0(AnimationSet)

	void TransformTrack::getTransformAt(Float32 _localTime, Matrix4x4& _matTransform)
	{

	}

	void TransformTrack::addTransformKey(TransformTrack& _transformKey)
	{

	}

	void TransformTrack::removeTransformKey(Int32 index)
	{

	}

	void TransformTrack::sortKeys()
	{

	}

	void PoseSQT::toMatrix(Matrix4x4& mat)
	{
		mat = Matrix4x4();
		mat.fromQuaternion(Rotation);
		mat.scale(Scale);
		mat.translate(Translation);
	}

	void AnimationHelper::LerpPoseSQT(PoseSQT& res, const PoseSQT& pose1, const PoseSQT& pose2, float alpha)
	{
		res.Translation = MathOps::VLerp(pose1.Translation, pose2.Translation, alpha);
		res.Rotation = MathOps::QSlerp(pose1.Rotation, pose2.Rotation, alpha);
		res.Scale = MathOps::VLerp(pose1.Scale, pose2.Scale, alpha);
	}

	void AnimationHelper::LerpAnimPose(AnimationPose& res, AnimationPose& pose1, AnimationPose& pose2, float alpha)
	{
		res.JointPoses.clear();
		
		ZASSERT(pose1.JointPoses.length() == pose2.JointPoses.length(), "Poses is not for the same skeleton");

		for (int i = 0; i < pose1.JointPoses.length(); i++)
		{
			PoseSQT poseSQT;
			LerpPoseSQT(poseSQT, pose1.JointPoses[i], pose2.JointPoses[i], alpha);
			res.JointPoses.push_back(poseSQT);
		}
	}

	void AnimationHelper::LerpAnimPartialPose(AnimationPose& res, AnimationPose& target, AnimationPose& pose, Skeleton* skelDef, Int32 boneIndex, float alpha, bool bBlendOrientation)
	{
		res.JointPoses.clear();

		ZASSERT(target.JointPoses.length() == pose.JointPoses.length(), "Poses is not for the same skeleton");

		Array<bool> boneMaskMap(skelDef->getJointCount());
		SkeletonJoint jointTemp;

		for (int i = 0; i < target.JointPoses.length(); i++)
		{
			skelDef->getJoint(i, jointTemp);
			if (jointTemp.ParentIndex == boneIndex || i == boneIndex)
			{
				boneMaskMap[i] = true;
			}
			else if (jointTemp.ParentIndex >= 0)
			{
				boneMaskMap[i] = boneMaskMap[jointTemp.ParentIndex];
			}
			else
			{
				boneMaskMap[i] = false;
			}

			if (boneMaskMap[i])
			{
				PoseSQT poseSQT;
				LerpPoseSQT(poseSQT, target.JointPoses[i], pose.JointPoses[i], alpha);
				res.JointPoses.push_back(poseSQT);
			}
			else
			{
				res.JointPoses.push_back(target.JointPoses[i]);
			}
		}

		if (bBlendOrientation && boneIndex > 0)
		{
			skelDef->getJoint(boneIndex, jointTemp);
			Int32 parentIndex = jointTemp.ParentIndex;
			Quaternion addRot = pose.JointPoses[parentIndex].Rotation *(*target.JointPoses[parentIndex].Rotation);
			res.JointPoses[boneIndex].Rotation = res.JointPoses[boneIndex].Rotation * addRot;
		}
	}

	void AnimationHelper::LerpAdditivePose(AnimationPose& res, AnimationPose& target, AnimationPose& addPose, float alpha)
	{
		res.JointPoses.clear();

		ZASSERT(target.JointPoses.length() == addPose.JointPoses.length(), "Poses is not for the same skeleton");

		for (int i = 0; i < target.JointPoses.length(); i++)
		{
			PoseSQT afterAddPose;
			afterAddPose.Translation = target.JointPoses[i].Translation + addPose.JointPoses[i].Translation;
			afterAddPose.Rotation = target.JointPoses[i].Rotation * addPose.JointPoses[i].Rotation;
			afterAddPose.Scale = target.JointPoses[i].Scale * addPose.JointPoses[i].Scale;

			PoseSQT poseSQT;
			LerpPoseSQT(poseSQT, target.JointPoses[i], afterAddPose, alpha);
			res.JointPoses.push_back(poseSQT);
		}
	}

}