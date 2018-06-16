#include "Animation.h"

#include "Math/MathOps.h"

namespace ZE
{

	IMPLEMENT_CLASS_0(AnimationClip)

	void AnimationClip::getAnimationPoseAtTime(float _localTime, AnimationPose& outPose)
	{
		// TODO Find the current index and next index
		Int32 cIndex, nextIndex;

		float portion = _localTime / m_duration;
		cIndex = portion * (m_frameCount - 1);
		nextIndex = cIndex + 1;
		float fraction = (portion * (m_frameCount - 1)) - cIndex;

		// Interpolate between SQT
		AnimationPose& cPose = m_animationSamples[cIndex];
		AnimationPose& nextPose = m_animationSamples[nextIndex];

		if (cIndex == 30)
		{
			int a = 10;
		}

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

}