#ifndef __ZE_RENDER_COMPONENT__
#define __ZE_RENDER_COMPONENT__

#include "SceneComponent.h"

namespace ZE {
	
	class Mesh;
	class Event;

	class RenderComponent : public SceneComponent {
		
		DEFINE_CLASS(RenderComponent)

	public:
		RenderComponent(GameContext* gameContext) : SceneComponent(gameContext), m_mesh(nullptr), m_bHighlight(false), m_bStatic(true) {}
		virtual ~RenderComponent() {}

		virtual void setupComponent() override;
		void handleGatherRender(Event* pEvent);

		void fromFile(const char* filePath);

		void setStatic(bool _bStatic);
		void setPhysicsEnabled(bool _bEnabled);
		void handlePhysicsUpdateTransform(Event* pEvent);

		void setupPhysics();

		Mesh* m_mesh;
		bool m_bHighlight;
		bool m_bStatic;
		bool m_physicsEnabled;
		Handle hPhysicsBody;
	};
}
#endif
