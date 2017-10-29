#include "CameraComponent.h"

#include "../Math/Vector3.h"

#define USING_INVERSE 0

namespace ZE {

	IMPLEMENT_CLASS_1(CameraComponent, SceneComponent);

	CameraComponent::CameraComponent(GameContext* gameContext)
		: SceneComponent(gameContext)
	{
		m_far = 1000.0f;
		m_near = 0.1f;
	}

	void CameraComponent::getViewMatrix(Matrix4x4& _outMat)
	{
#if USING_INVERSE
		_outMat = m_worldTransform.inverse();
#else
		_outMat.setU(m_worldTransform.getU());
		_outMat.setV(m_worldTransform.getV());
		_outMat.setN(m_worldTransform.getN());
		_outMat = _outMat.transpose();
		
		Vector3 v(-m_worldTransform.getU().dotProduct(m_worldTransform.getPos()),
			-m_worldTransform.getV().dotProduct(m_worldTransform.getPos()),
			-m_worldTransform.getN().dotProduct(m_worldTransform.getPos()));

		_outMat.setPos(v);
#endif
	}

}