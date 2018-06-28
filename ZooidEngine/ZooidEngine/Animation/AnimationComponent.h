#ifndef __ZE_ANIMATION_COMP_H__
#define __ZE_ANIMATION_COMP_H__

#include "GameObjectModel/Component.h"
#include "Animation/Animation.h"

namespace ZE
{
	class GameContext;
	class AnimationComponent : public Component
	{
		DEFINE_CLASS(AnimationComponent);

	public:

		AnimationComponent(GameContext* _gameContext);

		virtual ~AnimationComponent() {}

		virtual void setupComponent();

		// Handle Event Update
		virtual void handleEventUpdate(Event* evt);

		// Play animation clip
		// play rate: rate of animation, can be negative -- play backward
		void playAnimationClip(AnimationClip* _clip, bool _loop, Float32 _playRate);

		FORCEINLINE Float32 getPlayRate() const { return m_playRate; }

		FORCEINLINE bool isPlaying() const { return m_bPlaying; }

		FORCEINLINE bool isLooping() const { return m_bLoop; }

		FORCEINLINE AnimationClip* getCurrentClip() const { return m_currentClip; }

	protected:
		Float32 m_currentLocalTime;
		Float32 m_playRate;
		bool m_bPlaying;
		bool m_bLoop;
		AnimationClip* m_currentClip;
		Component* m_renderComponent;
	};
}
#endif
