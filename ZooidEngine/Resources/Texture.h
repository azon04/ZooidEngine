#ifndef __ZE_TEXTURE_H__
#define __ZE_TEXTURE_H__

#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"
#include "GameObjectModel/Object.h"

namespace ZE
{
	enum TextureWrap : UInt8
	{
		CLAMP_TO_EDGE,
		MIRRORED_REPEAT,
		REPEAT
	};

	enum TextureFilter : UInt8
	{
		NEAREST,
		LINEAR,
		NEAREST_MIPMAP_NEAREST,
		LINEAR_MIPMAP_NEAREST,
		NEAREST_MIPMAP_LINEAR,
		LINEAR_MIPMAP_LINEAR
	};

	class Texture : public Object
	{
		DEFINE_CLASS(Texture)

	public:
		Texture() : m_wrapU(MIRRORED_REPEAT), 
			m_wrapV(MIRRORED_REPEAT), 
			m_wrapN(MIRRORED_REPEAT),
			m_minFilter(LINEAR_MIPMAP_LINEAR),
			m_magFilter(LINEAR),
			m_bGenerateMipMap(true)
		{}

		// load texture from file
		static Handle loadTexture(const char* filePath);

		// load texture from memory buffer
		void loadFromBuffer(void* buffer, UInt32 _width, UInt32 _height, UInt32 _channelCount);

		// Get pointer to image
		FORCEINLINE UChar* getImage() const { return m_image; }

		FORCEINLINE UInt32 getWidth() const { return m_width; }
		FORCEINLINE UInt32 getHeight() const { return m_height; }
		
		FORCEINLINE UInt32 getChannel() const { return m_channel; }

		void release();

		// Set wrap on U
		void setWrapOnU(TextureWrap _uWrap) { m_wrapU = _uWrap; }

		// Set wrap on V
		void setWrapOnV(TextureWrap _vWrap) { m_wrapV = _vWrap; }

		// Set wrap on N
		void setWrapOnN(TextureWrap _nWrap) { m_wrapN = _nWrap; }

		// Set minified filter
		void setMinFilter(TextureFilter _minFilter) { m_minFilter = _minFilter; }

		// Set magnified filter
		void setMagFilter(TextureFilter _magFilter) { m_magFilter = _magFilter; }

		// Set generate mipmap
		void setGenerateMipmap(bool _bGenerateMipMap) { m_bGenerateMipMap = _bGenerateMipMap; }

		// Getter
		FORCEINLINE TextureWrap getWrapU() const { return m_wrapU; }
		FORCEINLINE TextureWrap getWrapV() const { return m_wrapV; }
		FORCEINLINE TextureWrap getWrapN() const { return m_wrapN; }

		FORCEINLINE TextureFilter getMinFilter() const { return m_minFilter; }
		FORCEINLINE TextureFilter getMagFilter() const { return m_magFilter; }

		FORCEINLINE bool isGenerateMipMap() const { return m_bGenerateMipMap; }

	protected:
		UChar* m_image;
		UInt32 m_width;
		UInt32 m_height;
		UInt32 m_channel;

		TextureWrap m_wrapU;
		TextureWrap m_wrapV;
		TextureWrap m_wrapN;

		TextureFilter m_minFilter;
		TextureFilter m_magFilter;

		bool m_bGenerateMipMap;
	};
};
#endif
