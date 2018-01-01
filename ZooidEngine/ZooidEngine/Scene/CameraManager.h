#ifndef __ZE_CAMERA_MANAGER_H_
#define __ZE_CAMERA_MANAGER_H_

#include "../GameObjectModel/Component.h"

namespace ZE {
	
	class CameraComponent;
	
	class CameraManager : public Component {

		DEFINE_CLASS(CameraManager)
	public:
		CameraManager(GameContext* gameContext) : Component(gameContext) {}
		virtual ~CameraManager() {}

		static void Init(GameContext* _gameContext);
		static void Destroy();

		static CameraManager* GetInstance();

		CameraComponent* getCurrentCamera() const { return m_currentCamera; }

	public:
		static CameraManager* s_instance;
		
		Array<Handle, true> m_cameras;
		
		CameraComponent* m_currentCamera;
	};
}
#endif
