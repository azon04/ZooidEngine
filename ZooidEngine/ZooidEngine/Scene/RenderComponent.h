#ifndef __ZE_RENDER_COMPONENT__
#define __ZE_RENDER_COMPONENT__

#include "SceneComponent.h"
#include "Physics/PhysicsBodyHolder.h"

namespace ZE {
	
	class Mesh;
	class Event;

	class RenderComponent : public SceneComponent , public IPhysicsBodyHolder
	{
		
		DEFINE_CLASS(RenderComponent)

	public:
		RenderComponent(GameContext* gameContext) : SceneComponent(gameContext), 
			m_mesh(nullptr), m_bHighlight(false), 
			m_bStatic(true), m_bTriggerOnly(false),
			m_bEnableGravity(true)
		{}

		virtual ~RenderComponent() {}

		virtual void setupComponent() override;
		void handleGatherRender(Event* pEvent);

		void fromFile(const char* filePath);

		void setTriggerOnly(bool _bTriggerOnly);
		void setStatic(bool _bStatic);
		void setPhysicsEnabled(bool _bEnabled);
		void handlePhysicsUpdateTransform(Event* pEvent);

		void setupPhysics();
		void setupMesh();

		// Implement from IPhysicsBodyHolder
		virtual bool hasPhysicsBody();
		virtual IPhysicsBody* getPhysicsBody();

		Mesh* m_mesh;
		bool m_bHighlight;

		// Physics Data
		bool m_bStatic;
		bool m_physicsEnabled;
		bool m_bTriggerOnly;
		bool m_bEnableGravity;
		Handle m_hPhysicsBody;

		// Skeleton Data
		Handle m_hSkeletonState;
	};
}
#endif
