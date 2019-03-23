#ifndef __ZE_RENDER_COMPONENT__
#define __ZE_RENDER_COMPONENT__

#include "SceneComponent.h"
#include "Physics/PhysicsBodyHolder.h"

namespace ZE 
{	
	class Mesh;
	class Event;

	class RenderComponent : public SceneComponent , public IPhysicsBodyHolder
	{
		
		DEFINE_CLASS(RenderComponent)

	public:
		RenderComponent(GameContext* gameContext) : SceneComponent(gameContext), 
			m_mesh(nullptr), m_bHighlight(false), 
			m_bStatic(true), m_bTriggerOnly(false),
			m_bEnableGravity(true), m_physicsEnabled(true),
			m_bCastShadow(true)
		{}

		virtual ~RenderComponent() {}

		virtual void setupComponent() override;
		
		// Handle Gather event
		void handleGatherRender(Event* pEvent);

		// Load mesh from File
		void loadMeshFromFile(const char* filePath);

		// Physics: Set this render object as trigger only
		void setTriggerOnly(bool _bTriggerOnly);

		// Physics: set this render component as static component
		void setStatic(bool _bStatic);

		void setPhysicsEnabled(bool _bEnabled);
		
		// Physics: handle physics update transform event
		void handlePhysicsUpdateTransform(Event* pEvent);

		void setupPhysics();
		void setupMesh();

		// Implement from IPhysicsBodyHolder
		virtual bool hasPhysicsBody();
		virtual IPhysicsBody* getPhysicsBody();

		FORCEINLINE Handle getSkeletonStateHandle() { return m_hSkeletonState; }

	protected:
		Mesh* m_mesh;
		bool m_bHighlight;

		// Physics Data
		bool m_bStatic;
		bool m_physicsEnabled;
		bool m_bTriggerOnly;
		bool m_bEnableGravity;
		Handle m_hPhysicsBody;

		// Shadow configuration
		bool m_bCastShadow;

		// Skeleton Data
		Handle m_hSkeletonState;
	};
}
#endif
