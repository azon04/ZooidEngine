#include "CameraComponent.h"

#include "Math/MathOps.h"
#include "Math/Vector3.h"
#include "Renderer/IRenderer.h"

#include "ZEGameContext.h"

#define USING_INVERSE 1

namespace ZE 
{

	IMPLEMENT_CLASS_1(CameraComponent, SceneComponent);

	CameraComponent::CameraComponent(GameContext* gameContext)
		: SceneComponent(gameContext), m_bUsingOrthoProjection(false)
	{
		m_far = 100.0f;
		m_near = 0.1f;

		m_orthoWidth = 5.0f;
	}

	void CameraComponent::getViewMatrix(Matrix4x4& _outMat)
	{
		updateCacheMatrix();
#if USING_INVERSE
		_outMat = m_cacheWorldMatrix.inverse();
#else
		_outMat.setU(m_cacheWorldMatrix.getU());
		_outMat.setV(m_cacheWorldMatrix.getV());
		_outMat.setN(m_cacheWorldMatrix.getN());
		_outMat = _outMat.transpose();
		
		Vector3 v(-m_cacheWorldMatrix.getU().dotProduct(m_cacheWorldMatrix.getPos()),
			-m_cacheWorldMatrix.getV().dotProduct(m_cacheWorldMatrix.getPos()),
			-m_cacheWorldMatrix.getN().dotProduct(m_cacheWorldMatrix.getPos()));

		_outMat.setPos(v);
#endif
	}

	void CameraComponent::getProjectionMat(Matrix4x4& _outMat)
	{
		_outMat = m_projectionMat;
	}

	void CameraComponent::calculateProjectionMat()
	{
		if (m_bUsingOrthoProjection)
		{
			MathOps::CreateOrthoProj(m_projectionMat, m_orthoWidth / 2.0f,
				(m_gameContext->getRenderer()->GetHeight() / m_gameContext->getRenderer()->GetWidth()) * m_orthoWidth * 0.5f, m_near, m_far);
		}
		else
		{
			MathOps::CreatePerspectiveProjEx(m_projectionMat,
				m_gameContext->getRenderer()->GetWidth() / m_gameContext->getRenderer()->GetHeight(), 45.0f, m_near, m_far);

		}
	}

}