#ifndef __ZE_DEBUG_CAMERA_H__
#define __ZE_DEBUG_CAMERA_H__

#include "CameraComponent.h"
#include "Events/Events.h"

namespace ZE
{
	class DebugCamera : public CameraComponent
	{
		DEFINE_CLASS(DebugCamera)

	public:
		DebugCamera(GameContext* gameContext) : CameraComponent(gameContext)
		{}

		// CameraComponent implementation
		virtual void setupComponent() override;
		virtual void handleUpdateEvent(Event* _event) override;

		// Handle key down event
		void handleKeyDownEvent(Event* _event);

		// Handle key up event
		void handleKeyUpEvent(Event* _event);

		// Handle mouse drag interaction
		void handleMouseDragEvent(Event* _event);

	protected:
		void processInput();

	protected:
		Vector3 m_velocity;
		Vector3 m_rotation;
	};
}

#endif
