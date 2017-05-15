#ifndef __ZE_COMPONENT_H__
#define __ZE_COMPONENT_H__

// Outer Engine includes
#include <vector>

#include "Object.h"

namespace ZE {
	class Component : public Object {

		DEFINE_CLASS(Component)

	public:
		Component() {}
		virtual ~Component() {}

		std::vector<Component*> m_components;
		std::vector<Component*> m_parents;
	};
}
#endif
