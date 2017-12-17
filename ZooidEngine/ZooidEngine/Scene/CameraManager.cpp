#include "CameraManager.h"
#include "DebugCamera.h"

#include "ZEGameContext.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(CameraManager, Component)

	CameraManager* CameraManager::s_instance = nullptr;

	void CameraManager::Init(GameContext* _gameContext)
	{
		Handle handle("CameraManager", sizeof(CameraManager));
		s_instance = new(handle) CameraManager(_gameContext);

		// TODO init some default debug camera
		Handle cameraHandle("DEBUG CAMERA", sizeof(DebugCamera));
		DebugCamera* debugCamera = new(cameraHandle) DebugCamera(_gameContext);
		debugCamera->setupComponent();
		s_instance->m_currentCamera = debugCamera;

		s_instance->m_cameras.push_back(cameraHandle);

		_gameContext->getRootComponent()->addChild(debugCamera);
	}

	ZE::CameraManager* CameraManager::GetInstance()
	{
		return s_instance;
	}

}