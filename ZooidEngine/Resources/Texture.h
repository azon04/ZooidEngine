#ifndef __ZE_TEXTURE_H__
#define __ZE_TEXTURE_H__

#include "Utils/PrimitiveTypes.h"
#include "Memory/Handle.h"
#include "GameObjectModel/Object.h"
#include "Renderer/Enums.h"
#include "Math/Vector4.h"
#include "Utils/Array.h"
#include "Renderer/Structs.h"

namespace ZE
{

	struct TextureCreateDesc
	{
		UInt32 Width;
		UInt32 Height;
		UInt32 Depth = 1;
		UInt32 FaceCount = 1;
		UInt32 Channel = 3;

		ETextureFormat TextureFormat = TEX_RGB;
		
		ETextureWrap WrapU = MIRRORED_REPEAT;
		ETextureWrap WrapV = MIRRORED_REPEAT;
		ETextureWrap WrapN = MIRRORED_REPEAT;

		ETextureFilter MinFilter = LINEAR_MIPMAP_LINEAR;
		ETextureFilter MagFilter = LINEAR;
		
		EDataType DataType = UNSIGNED_BYTE;
		bool bGenerateMipMap = true;

		Vector4 BorderColor;
		void* InitialData = nullptr;
	};

	class Texture : public Object
	{
		DEFINE_CLASS(Texture)

	public:
		Texture()
		{}

		// load texture from file
		static Handle loadTexture(const char* filePath);

		// load texture from memory buffer
		void loadFromBuffer(void* firstBuffer, UInt32 _width, UInt32 _height, UInt32 _depth, UInt32 _channelCount, UInt32 faces = 1);

		// load texture from memory buffer
		void loadFromBufferExt(void* firstBuffer, UInt32 _width, UInt32 _height, UInt32 _depth, UInt32 _channelCount, ETextureFormat textureFormat, UInt32 faces = 1);

		// Add buffer to texture faces
		void addBufferToTexture(void* buffer, UInt32 faceIndex);

		// create empty texture
		void createEmpty(UInt32 _width, UInt32 _height, UInt32 _depth, ETextureFormat _format, UInt32 _faces=1);

		// Get pointer to image
		FORCEINLINE UChar* getImage(UInt32 faceIndex = 0) const { return m_images[faceIndex]; }

		FORCEINLINE UInt32 getWidth() const { return m_textureDesc.Width; }
		FORCEINLINE UInt32 getHeight() const { return m_textureDesc.Height; }
		FORCEINLINE UInt32 getDepth() const { return m_textureDesc.Depth; }
		
		FORCEINLINE UInt32 getChannel() const { return m_textureDesc.Channel; }

		FORCEINLINE UInt32 getNumberOfFaces() const { return m_textureDesc.FaceCount; }

		void release();

		// Set wrap on U
		void setWrapOnU(ETextureWrap _uWrap) { m_textureDesc.WrapU = _uWrap; }

		// Set wrap on V
		void setWrapOnV(ETextureWrap _vWrap) { m_textureDesc.WrapV = _vWrap; }

		// Set wrap on N
		void setWrapOnN(ETextureWrap _nWrap) { m_textureDesc.WrapN = _nWrap; }

		// Set minified filter
		void setMinFilter(ETextureFilter _minFilter) { m_textureDesc.MinFilter = _minFilter; }

		// Set magnified filter
		void setMagFilter(ETextureFilter _magFilter) { m_textureDesc.MagFilter = _magFilter; }

		// Set generate mipmap
		void setGenerateMipmap(bool _bGenerateMipMap) { m_textureDesc.bGenerateMipMap = _bGenerateMipMap; }

		// Set Data type
		void setDataType(EDataType _dataType) { m_textureDesc.DataType = _dataType; }

		// Set Border Color in RGBA
		void setBorderColor(const Vector4& _color) { m_textureDesc.BorderColor = _color; }

		// Set Texture Format
		void setTextureFormat(ETextureFormat textureFormat) { m_textureDesc.TextureFormat = textureFormat; }

		// Getter
		FORCEINLINE ETextureWrap getWrapU() const { return m_textureDesc.WrapU; }
		FORCEINLINE ETextureWrap getWrapV() const { return m_textureDesc.WrapV; }
		FORCEINLINE ETextureWrap getWrapN() const { return m_textureDesc.WrapN; }

		FORCEINLINE ETextureFormat getTextureFormat() const { return m_textureDesc.TextureFormat; }
		FORCEINLINE ETextureFilter getMinFilter() const { return m_textureDesc.MinFilter; }
		FORCEINLINE ETextureFilter getMagFilter() const { return m_textureDesc.MagFilter; }
		FORCEINLINE EDataType getDataType() const { return m_textureDesc.DataType; }
		FORCEINLINE Vector4 getBorderColor() const { return m_textureDesc.BorderColor; }

		FORCEINLINE bool isGenerateMipMap() const { return m_textureDesc.bGenerateMipMap; }

		FORCEINLINE TextureCreateDesc getTextureDesc() { return m_textureDesc; }

	protected:
		Array<UChar*> m_images;

		TextureCreateDesc m_textureDesc;
	};
};
#endif
