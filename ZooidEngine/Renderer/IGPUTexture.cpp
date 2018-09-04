#include "IGPUTexture.h"

namespace ZE
{
	void IGPUTexture::fromTexture(Texture* texture)
	{
		m_textureRes = texture;
		m_width = texture->getWidth();
		m_height = texture->getHeight();
	}

	void IGPUTexture::release()
	{
	}

}