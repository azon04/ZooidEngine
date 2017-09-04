#ifndef __ZE_COMPONENT_H__
#define __ZE_COMPONENT_H__

// Outer Engine includes

#include "../Utils/Array.h"
#include "Object.h"

namespace ZE {
	class GameContext;
	class Component : public Object {

		DEFINE_CLASS(Component)

	public:
		Component(GameContext* gameContext) : m_gameContext(gameContext){}
		virtual ~Component() {}

		Array<Component*, true> m_components;
		Array<Component*, true> m_parents;

	protected:
		GameContext* m_gameContext;
	};
}
#endif
