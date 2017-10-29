#ifndef __ZE_RENDER_COMPONENT__
#define __ZE_RENDER_COMPONENT__

#include "../GameObjectModel/Component.h"

namespace ZE {
	class RenderComponent : public Component {
		DEFINE_CLASS(RenderComponent)

	public:
		RenderComponent(GameContext* gameContext) : Component(gameContext) {}
		virtual ~RenderComponent() {}
	};
}
#endif
