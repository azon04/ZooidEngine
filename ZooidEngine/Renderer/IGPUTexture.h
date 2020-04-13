#ifndef __ZE_GPU_TEXTURE_H__
#define	__ZE_GPU_TEXTURE_H__

#include "Resources/Texture.h"

namespace ZE
{
	class IGPUTexture
	{
	public:
		IGPUTexture()
		{
		}

		virtual ~IGPUTexture() { release(); }

		// From CPU Texture
		virtual void fromTexture(Texture* texture);

		// Create using TextureCreateDesc
		virtual void create(const TextureCreateDesc& createDesc) = 0;

		// Release GPU Texture
		virtual void release();

		// bind texture to rendering pipeline
		virtual void bind() = 0;

		// unbind texture from rendering pipeline
		virtual void unbind() = 0;

		// Get height of the texture in pixel
		UInt32 getHeight() const { return m_height; }

		// Get Wwidth of the texture in pixel
		UInt32 getWidth() const  { return m_width; }

		// Is Cube Map
		bool isCubeTexture() const { return m_face == 6; }

	protected:
		Texture* m_textureRes;
		UInt32 m_width;
		UInt32 m_height;
		UInt32 m_depth;
		UInt32 m_face;
	};
}
#endif
