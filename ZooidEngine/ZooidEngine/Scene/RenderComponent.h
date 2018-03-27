#ifndef __ZE_RENDER_COMPONENT__
#define __ZE_RENDER_COMPONENT__

#include "SceneComponent.h"

namespace ZE {
	
	class Mesh;
	class Event;

	class RenderComponent : public SceneComponent {
		
		DEFINE_CLASS(RenderComponent)

	public:
		RenderComponent(GameContext* gameContext) : SceneComponent(gameContext), m_mesh(nullptr), m_bHighlight(false) {}
		virtual ~RenderComponent() {}

		virtual void setupComponent() override;
		void handleGatherRender(Event* pEvent);

		void fromFile(const char* filePath);

		Mesh* m_mesh;
		bool m_bHighlight;
	};
}
#endif
