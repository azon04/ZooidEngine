#include "CameraManager.h"
#include "DebugCamera.h"

#include "Renderer/DrawList.h"
#include "Renderer/IRenderer.h"
#include "Math/MathOps.h"

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
		debugCamera->setWorldPosition(Vector3(2.0f, 2.0f, 5.0f));
		debugCamera->rotateInDeg(Vector3(-15.0f, 15.0f, 0.0f));
		debugCamera->setupComponent();
		s_instance->setCurrentCamara(debugCamera);

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

	void CameraManager::setCurrentCamara(CameraComponent* cameraComponent)
	{
		m_currentCamera = cameraComponent;
		m_currentCamera->calculateProjectionMat();
	}

}