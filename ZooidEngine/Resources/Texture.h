#ifndef __ZE_TEXTURE_H__
#define __ZE_TEXTURE_H__

#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"
#include "GameObjectModel/Object.h"
#include "Renderer/Enums.h"
#include "Math/Vector4.h"
#include "Utils/Array.h"

namespace ZE
{
	class Texture : public Object
	{
		DEFINE_CLASS(Texture)

	public:
		Texture() : m_wrapU(MIRRORED_REPEAT), 
			m_wrapV(MIRRORED_REPEAT), 
			m_wrapN(MIRRORED_REPEAT),
			m_minFilter(LINEAR_MIPMAP_LINEAR),
			m_magFilter(LINEAR),
			m_bGenerateMipMap(true),
			m_face(1),
			m_depth(1)
		{}

		// load texture from file
		static Handle loadTexture(const char* filePath);

		// load texture from memory buffer
		void loadFromBuffer(void* firstBuffer, UInt32 _width, UInt32 _height, UInt32 _depth, UInt32 _channelCount, UInt32 faces = 1);

		// Add buffer to texture faces
		void addBufferToTexture(void* buffer, UInt32 faceIndex);

		// create empty texture
		void createEmpty(UInt32 _width, UInt32 _height, UInt32 _depth, ETextureFormat _format, UInt32 _faces=1);

		// Get pointer to image
		FORCEINLINE UChar* getImage(UInt32 faceIndex = 0) const { return m_images[faceIndex]; }

		FORCEINLINE UInt32 getWidth() const { return m_width; }
		FORCEINLINE UInt32 getHeight() const { return m_height; }
		FORCEINLINE UInt32 getDepth() const { return m_depth; }
		
		FORCEINLINE UInt32 getChannel() const { return m_channel; }

		FORCEINLINE UInt32 getNumberOfFaces() const { return m_face; }

		void release();

		// Set wrap on U
		void setWrapOnU(ETextureWrap _uWrap) { m_wrapU = _uWrap; }

		// Set wrap on V
		void setWrapOnV(ETextureWrap _vWrap) { m_wrapV = _vWrap; }

		// Set wrap on N
		void setWrapOnN(ETextureWrap _nWrap) { m_wrapN = _nWrap; }

		// Set minified filter
		void setMinFilter(ETextureFilter _minFilter) { m_minFilter = _minFilter; }

		// Set magnified filter
		void setMagFilter(ETextureFilter _magFilter) { m_magFilter = _magFilter; }

		// Set generate mipmap
		void setGenerateMipmap(bool _bGenerateMipMap) { m_bGenerateMipMap = _bGenerateMipMap; }

		// Set Data type
		void setDataType(EDataType _dataType) { m_dataType = _dataType; }

		// Set Border Color in RGBA
		void setBorderColor(const Vector4& _color) { m_borderColor = _color; }

		// Getter
		FORCEINLINE ETextureWrap getWrapU() const { return m_wrapU; }
		FORCEINLINE ETextureWrap getWrapV() const { return m_wrapV; }
		FORCEINLINE ETextureWrap getWrapN() const { return m_wrapN; }

		FORCEINLINE ETextureFormat getTextureFormat() const { return m_format; }
		FORCEINLINE ETextureFilter getMinFilter() const { return m_minFilter; }
		FORCEINLINE ETextureFilter getMagFilter() const { return m_magFilter; }
		FORCEINLINE EDataType getDataType() const { return m_dataType; }
		FORCEINLINE Vector4 getBorderColor() const { return m_borderColor; }

		FORCEINLINE bool isGenerateMipMap() const { return m_bGenerateMipMap; }

	protected:
		Array<UChar*> m_images;
		UInt32 m_width;
		UInt32 m_height;
		UInt32 m_depth;
		UInt32 m_face;
		UInt32 m_channel;
		Vector4 m_borderColor; // in RGBA

		ETextureFormat m_format;

		ETextureWrap m_wrapU;
		ETextureWrap m_wrapV;
		ETextureWrap m_wrapN;

		ETextureFilter m_minFilter;
		ETextureFilter m_magFilter;

		EDataType m_dataType = UNSIGNED_BYTE;

		bool m_bGenerateMipMap;
	};
};
#endif
