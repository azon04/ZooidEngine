#include "IGPUTexture.h"

namespace ZE
{
	void IGPUTexture::fromTexture(Texture* texture)
	{
		m_textureRes = texture;
		m_width = texture->getWidth();
		m_height = texture->getHeight();
		m_face = texture->getNumberOfFaces();
		m_depth = texture->getDepth();
	}

	void IGPUTexture::release()
	{
	}

}