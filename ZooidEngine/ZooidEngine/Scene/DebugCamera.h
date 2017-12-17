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

		virtual void setupComponent() override;
		virtual void handleUpdateEvent(Event* _event) override;

		void handleKeyDownEvent(Event* _event);
		void handleKeyUpEvent(Event* _event);

	protected:
		Vector3 m_velocity;
		Vector3 m_rotation;
	};
}

#endif
