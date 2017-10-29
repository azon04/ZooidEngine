#ifndef __ZE_ACTOR_H__

#include "../GameObjectModel/Component.h"

namespace ZE {
	/*
		Actor Class
		Actor is an game object that can be put inside the game world/game context
		It's inheritance from Component, so it can have components and also can be attached to another actor.
		This is reflect of Unreal Engine design
	*/
	class Actor : public Component {
		DEFINE_CLASS(Actor)

	public:
		Actor(GameContext* gameContext) : Component(gameContext) {}
		virtual ~Actor() {}
	
	protected:
		char* m_ActorName;
	
	};
}
#endif
