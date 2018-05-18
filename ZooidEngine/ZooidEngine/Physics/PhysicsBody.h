#ifndef __PHYSICS_BODY_H__
#define __PHYSICS_BODY_H__

#include "Physics.h"

namespace ZE
{
	class Object;

	class IPhysicsBody
	{
	public:
		IPhysicsBody() {}

		Object* getGameObject() { return m_gameObject; }
		void setGameObject(Object* _gameObject) { m_gameObject = _gameObject; }

	protected:
		Object* m_gameObject;
	};
}
#endif
