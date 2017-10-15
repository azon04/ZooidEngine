#include "CameraManager.h"
#include "CameraComponent.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(CameraManager, Component)

	CameraManager* CameraManager::s_instance = nullptr;

	void CameraManager::Init(GameContext* _gameContext)
	{
		Handle handle("CameraManager", sizeof(CameraManager));
		s_instance = new(handle) CameraManager(_gameContext);

		// TODO init some default debug camera
		Handle cameraHandle("DEBUG CAMERA", sizeof(CameraComponent));
		CameraComponent* debugCamera = new(cameraHandle) CameraComponent(_gameContext);
		s_instance->m_currentCamera = debugCamera;

		s_instance->m_cameras.push_back(cameraHandle);
	}

	ZE::CameraManager* CameraManager::GetInstance()
	{
		return s_instance;
	}

}