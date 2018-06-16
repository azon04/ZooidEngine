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

		virtual void handleEventUpdate(Event* evt);

		void playAnimationClip(AnimationClip* _clip, bool _loop, Float32 _playRate);

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
