#ifndef __ZE_UI_INPUT_COMPONENT_H__
#define __ZE_UI_INPUT_COMPONENT_H__

#include "GameObjectModel/Component.h"
#include "Events/Events.h"
#include "Events/InputEvents.h"

namespace ZE
{
	class UIInputComponent : public Component
	{
		DEFINE_CLASS(UIInputComponent)

	public:

		UIInputComponent(GameContext* gameContext);
		virtual ~UIInputComponent() {}

		virtual void setupComponent() override;

	protected:
		// Handle Update Component
		void handleUpdateEvent(Event* _event);
	};
}
#endif
