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
		debugCamera->m_worldTransform.setPos(Vector3(2.0f, 5.0f, 15.0f));
		debugCamera->m_worldTransform.rotateAroundV(DegToRad(15));
		debugCamera->m_worldTransform.rotateAroundU(DegToRad(-15));
		debugCamera->setupComponent();
		s_instance->m_currentCamera = debugCamera;

		s_instance->m_cameras.push_back(cameraHandle);

		_gameContext->getRootComponent()->addChild(debugCamera);
	}

	void CameraManager::Destroy()
	{

	}

	ZE::CameraManager* CameraManager::GetInstance()
	{
		return s_instance;
	}

}