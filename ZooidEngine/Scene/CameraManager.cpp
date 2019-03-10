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
		debugCamera->setWorldPosition(Vector3(2.0f, 5.0f, 15.0f));
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
		if (cameraComponent->m_bUsingOrthoProjection)
		{
			MathOps::CreateOrthoProj(m_gameContext->getDrawList()->m_projectionMat, m_currentCamera->m_orthoWidth / 2.0f, 
				(m_gameContext->getRenderer()->GetHeight() / m_gameContext->getRenderer()->GetWidth()) * m_currentCamera->m_orthoWidth * 0.5f, m_currentCamera->m_near, m_currentCamera->m_far);
		
			// #TODO set ViewFustrum for Orthographic Projection
		}
		else
		{
			MathOps::CreatePerspectiveProjEx(m_gameContext->getDrawList()->m_projectionMat, 
				m_gameContext->getRenderer()->GetWidth() / m_gameContext->getRenderer()->GetHeight(), 45.0f, m_currentCamera->m_near, m_currentCamera->m_far);

			m_gameContext->getDrawList()->m_viewFustrum.setProjectionVars(m_gameContext->getRenderer()->GetWidth() / m_gameContext->getRenderer()->GetHeight(), 45.0f, m_currentCamera->m_near, m_currentCamera->m_far);
		}
	}

}