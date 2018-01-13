#ifndef __ZE_RENDER_COMPONENT__
#define __ZE_RENDER_COMPONENT__

#include "../GameObjectModel/Component.h"

namespace ZE {
	
	class Mesh;

	class RenderComponent : public Component {
		DEFINE_CLASS(RenderComponent)

	public:
		RenderComponent(GameContext* gameContext) : Component(gameContext), m_mesh(nullptr) {}
		virtual ~RenderComponent() {}

		Mesh* m_mesh;
	};
}
#endif
