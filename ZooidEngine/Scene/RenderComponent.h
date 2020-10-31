#ifndef __ZE_RENDER_COMPONENT__
#define __ZE_RENDER_COMPONENT__

#include "Math/Shapes.h"
#include "SceneComponent.h"
#include "Physics/PhysicsBodyHolder.h"

namespace ZE 
{	
	class Mesh;
	class Material;
	class Event;

	class RenderComponent : public SceneComponent , public IPhysicsBodyHolder
	{
		
		DEFINE_CLASS(RenderComponent)

	public:
		RenderComponent(GameContext* gameContext) : SceneComponent(gameContext), 
			m_mesh(nullptr), m_material(nullptr), m_bHighlight(false), 
			m_bStatic(true), m_bTriggerOnly(false),
			m_bEnableGravity(true), m_physicsEnabled(true),
			m_bCastShadow(true)
		{}

		virtual ~RenderComponent() {}

		virtual void setupComponent() override;
		
		// Handle Gather event
		void handleGatherRender(Event* pEvent);

		// Handle Gather render bound
		void handleGatherRenderBound(Event* pEvent);

		// Handle Gather Shadow List
		void handleGatherShadowRender(Event* pEvent);

		// Load mesh from File
		void loadMeshFromFile(const char* filePath);

		// Physics: Set this render object as trigger only
		void setTriggerOnly(bool _bTriggerOnly);

		// Physics: set this render component as static component
		void setStatic(bool _bStatic);

		void setPhysicsEnabled(bool _bEnabled);

		void setMaterial(Material* material);

		void setHighLight(bool _highlight) { m_bHighlight = _highlight; }
		
		// Physics: handle physics update transform event
		void handlePhysicsUpdateTransform(Event* pEvent);

		void setupPhysics();
		void setupMesh();

		// Implement from IPhysicsBodyHolder
		virtual bool hasPhysicsBody();
		virtual IPhysicsBody* getPhysicsBody();

		FORCEINLINE Handle getSkeletonStateHandle() { return m_hSkeletonState; }
	protected:

		virtual void updateCacheMatrix() override;

	protected:
		Mesh* m_mesh;
		Material* m_material;

		// Physics Data
		Handle m_hPhysicsBody;

		// Skeleton Data
		Handle m_hSkeletonState;

		// Bounding Information
		Sphere m_boundingSphere;
		AxisAlignedBox m_boundingBox;

		// Flags
		bool m_bHighlight:1;
		bool m_bStatic:1;
		bool m_physicsEnabled:1;
		bool m_bTriggerOnly:1;
		bool m_bEnableGravity:1;

		// Shadow configuration
		bool m_bCastShadow:1;

	};
}
#endif
