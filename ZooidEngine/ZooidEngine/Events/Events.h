#ifndef __ZE_EVENTS__
#define __ZE_EVENTS__

#include "GameObjectModel/Object.h"

namespace ZE {
	
	class GameContext;

	class Event : public Object
	{
		DEFINE_CLASS(Event);

	public:
		Event() {}
		virtual ~Event() {}

	};
};

#endif 
