#include "Animation.h"

#include "Math/MathOps.h"
#include "Skeleton.h"

namespace ZE
{

	IMPLEMENT_CLASS_0(AnimationClip)

	void AnimationClip::getAnimationPoseAtTime(float _localTime, AnimationPose& outPose)
	{
		// Wrap the time by duration
		while (_localTime > m_duration)
		{
			_localTime = _localTime - m_duration;
		}

		// TODO Find the current index and next index
		Int32 cIndex, nextIndex;

		float portion = _localTime / m_duration;
		cIndex = portion * m_frameCount;
		nextIndex = (cIndex + 1) % m_frameCount;
		float fraction = (portion * (m_frameCount)) - cIndex;

		// Interpolate between SQT
		AnimationPose& cPose = m_animationSamples[cIndex];
		AnimationPose& nextPose = m_animationSamples[nextIndex];

		for (int i = 0; i < cPose.jointPoses.length(); i++)
		{
			outPose.jointPoses.push_back(PoseSQT());
			PoseSQT& outPoseSQT = outPose.jointPoses[i];
			PoseSQT& cSQT = cPose.jointPoses[i];
			PoseSQT& nextSQT = nextPose.jointPoses[i];

			outPoseSQT.translation = MathOps::VLerp(cSQT.translation, nextSQT.translation, fraction);
			outPoseSQT.rotation = MathOps::QSlerp(cSQT.rotation, nextSQT.rotation, fraction);
			outPoseSQT.scale = MathOps::VLerp(cSQT.scale, nextSQT.scale, fraction);
		}
	}

	void AnimationClip::getAnimationPoseAtScaleTime(float _localTime, AnimationPose& outPose, float scale)
	{
		// Wrap the time by duration
		float scaledDuration = m_duration * scale;
		while (_localTime > scaledDuration)
		{
			_localTime = _localTime - scaledDuration;
		}

		// TODO Find the current index and next index
		Int32 cIndex, nextIndex;

		float portion = _localTime / scaledDuration;
		cIndex = portion * (m_frameCount);
		nextIndex = (cIndex + 1) % m_frameCount;
		float fraction = (portion * (m_frameCount)) - cIndex;

		// Interpolate between SQT
		AnimationPose& cPose = m_animationSamples[cIndex];
		AnimationPose& nextPose = m_animationSamples[nextIndex];

		for (int i = 0; i < cPose.jointPoses.length(); i++)
		{
			outPose.jointPoses.push_back(PoseSQT());
			PoseSQT& outPoseSQT = outPose.jointPoses[i];
			PoseSQT& cSQT = cPose.jointPoses[i];
			PoseSQT& nextSQT = nextPose.jointPoses[i];

			outPoseSQT.translation = MathOps::VLerp(cSQT.translation, nextSQT.translation, fraction);
			outPoseSQT.rotation = MathOps::QSlerp(cSQT.rotation, nextSQT.rotation, fraction);
			outPoseSQT.scale = MathOps::VLerp(cSQT.scale, nextSQT.scale, fraction);
		}
	}

	void AnimationClip::getAnimationPoseAtTimeWithDuration(float _localTime, AnimationPose& outPose, float _duration)
	{
		// Wrap the time by duration
		while (_localTime > _duration)
		{
			_localTime = _localTime - _duration;
		}

		// TODO Find the current index and next index
		Int32 cIndex, nextIndex;

		float portion = _localTime / _duration;
		cIndex = portion * (m_frameCount);
		nextIndex = (cIndex + 1) % m_frameCount;
		float fraction = (portion * (m_frameCount)) - cIndex;

		// Interpolate between SQT
		AnimationPose& cPose = m_animationSamples[cIndex];
		AnimationPose& nextPose = m_animationSamples[nextIndex];

		for (int i = 0; i < cPose.jointPoses.length(); i++)
		{
			outPose.jointPoses.push_back(PoseSQT());
			PoseSQT& outPoseSQT = outPose.jointPoses[i];
			PoseSQT& cSQT = cPose.jointPoses[i];
			PoseSQT& nextSQT = nextPose.jointPoses[i];

			outPoseSQT.translation = MathOps::VLerp(cSQT.translation, nextSQT.translation, fraction);
			outPoseSQT.rotation = MathOps::QSlerp(cSQT.rotation, nextSQT.rotation, fraction);
			outPoseSQT.scale = MathOps::VLerp(cSQT.scale, nextSQT.scale, fraction);
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
		mat.fromQuaternion(rotation);
		mat.scale(scale);
		mat.translate(translation);
	}

	void AnimationHelper::LerpPoseSQT(PoseSQT& res, const PoseSQT& pose1, const PoseSQT& pose2, float alpha)
	{
		res.translation = MathOps::VLerp(pose1.translation, pose2.translation, alpha);
		res.rotation = MathOps::QSlerp(pose1.rotation, pose2.rotation, alpha);
		res.scale = MathOps::VLerp(pose1.scale, pose2.scale, alpha);
	}

	void AnimationHelper::LerpAnimPose(AnimationPose& res, AnimationPose& pose1, AnimationPose& pose2, float alpha)
	{
		res.jointPoses.clear();
		
		ZASSERT(pose1.jointPoses.length() == pose2.jointPoses.length(), "Poses is not for the same skeleton");

		for (int i = 0; i < pose1.jointPoses.length(); i++)
		{
			PoseSQT poseSQT;
			LerpPoseSQT(poseSQT, pose1.jointPoses[i], pose2.jointPoses[i], alpha);
			res.jointPoses.push_back(poseSQT);
		}
	}

	void AnimationHelper::LerpAnimPartialPose(AnimationPose& res, AnimationPose& target, AnimationPose& pose, Skeleton* skelDef, Int32 boneIndex, float alpha, bool bBlendOrientation)
	{
		res.jointPoses.clear();

		ZASSERT(target.jointPoses.length() == pose.jointPoses.length(), "Poses is not for the same skeleton");

		Array<bool> boneMaskMap(skelDef->getJointCount());
		SkeletonJoint jointTemp;

		for (int i = 0; i < target.jointPoses.length(); i++)
		{
			skelDef->getJoint(i, jointTemp);
			if (jointTemp.parentIndex == boneIndex)
			{
				boneMaskMap[i] = true;
			}
			else if (jointTemp.parentIndex >= 0)
			{
				boneMaskMap[i] = boneMaskMap[jointTemp.parentIndex];
			}
			else
			{
				boneMaskMap[i] = false;
			}

			if (boneMaskMap[i])
			{
				PoseSQT poseSQT;
				LerpPoseSQT(poseSQT, target.jointPoses[i], pose.jointPoses[i], alpha);
				res.jointPoses.push_back(poseSQT);
			}
			else
			{
				res.jointPoses.push_back(target.jointPoses[i]);
			}
		}

		if (bBlendOrientation && boneIndex > 0)
		{
			skelDef->getJoint(boneIndex, jointTemp);
			Int32 parentIndex = jointTemp.parentIndex;
			Quaternion addRot = pose.jointPoses[parentIndex].rotation * (*target.jointPoses[parentIndex].rotation);
			addRot.normalize();
			res.jointPoses[boneIndex].rotation = res.jointPoses[boneIndex].rotation * addRot;
			res.jointPoses[boneIndex].rotation.normalize();
		}
	}

	void AnimationHelper::LerpAdditivePose(AnimationPose& res, AnimationPose& target, AnimationPose& addPose, float alpha)
	{
		res.jointPoses.clear();

		ZASSERT(target.jointPoses.length() == addPose.jointPoses.length(), "Poses is not for the same skeleton");

		for (int i = 0; i < target.jointPoses.length(); i++)
		{
			PoseSQT afterAddPose;
			afterAddPose.translation = target.jointPoses[i].translation + addPose.jointPoses[i].translation;
			afterAddPose.rotation = target.jointPoses[i].rotation * addPose.jointPoses[i].rotation;
			afterAddPose.scale = target.jointPoses[i].scale * addPose.jointPoses[i].scale;

			PoseSQT poseSQT;
			LerpPoseSQT(poseSQT, target.jointPoses[i], afterAddPose, alpha);
			res.jointPoses.push_back(poseSQT);
		}
	}

}