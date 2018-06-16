#include "AnimationComponent.h"

#include "Scene/RenderComponent.h"
#include "Animation/Skeleton.h"
#include "Events/Events.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(AnimationComponent, Component);

	AnimationComponent::AnimationComponent(GameContext* _gameContext) 
		: Component(_gameContext),
		m_currentClip(nullptr)
	{

	}

	void AnimationComponent::setupComponent()
	{
		Component::setupComponent();
		addEventDelegate(Event_UPDATE, &AnimationComponent::handleEventUpdate);
	}

	void AnimationComponent::handleEventUpdate(Event* evt)
	{
		Event_UPDATE* pEventUpdate = (Event_UPDATE*)evt;

		if (m_bPlaying && m_currentClip)
		{
			Float32 playTime;
			m_currentLocalTime += pEventUpdate->m_deltaTime / 1000.0f * m_playRate;
			if (m_bLoop)
			{
				if (m_playRate > 0.0f)
				{
					while (m_currentLocalTime > m_currentClip->m_duration)
					{
						m_currentLocalTime -= m_currentClip->m_duration;
					}
					playTime = m_currentLocalTime;
				}
				else
				{
					while (m_currentLocalTime < -m_currentClip->m_duration)
					{
						m_currentLocalTime += m_currentClip->m_duration;
					}
					playTime = m_currentClip->m_duration + m_currentLocalTime;
				}
			}
			else if(!m_bLoop)
			{
				m_bPlaying = false;
			}

			// TODO update animation state
			AnimationPose pose;
			m_currentClip->getAnimationPoseAtTime(playTime, pose);

			if (m_parents.length() > 0)
			{
				RenderComponent* pRenderComp = (RenderComponent*)m_parents[0];
				SkeletonState* pSkelState = pRenderComp->m_hSkeletonState.getObject<SkeletonState>();
				
				ZASSERT(pSkelState->getSkeleton() == m_currentClip->m_skeleton, "Skeleton using in render and animation didn't match");

				Array<Matrix4x4> mats(m_currentClip->m_skeleton->getJointCount());

				for (int i = 0; i < m_currentClip->m_skeleton->getJointCount(); i++)
				{
					mats.push_back(Matrix4x4());
					pose.jointPoses[i].toMatrix(mats[i]);
				}

				pSkelState->setJointStateMatrices(mats, true);
			}
		}
	}

	void AnimationComponent::playAnimationClip(AnimationClip* _clip, bool _loop, Float32 _playRate)
	{
		m_currentClip = _clip;
		m_currentLocalTime = 0.0f;
		m_bLoop = _loop;
		m_bPlaying = true;
		m_playRate = _playRate;
	}

}