#include "TextMesh.h"

#include "Font.h"
#include "ZEGameContext.h"

#include "Renderer/BufferData.h"
#include "Renderer/IGPUBufferData.h"

#include "ResourceManagers/BufferManager.h"

#include "Utils/Array.h"

namespace ZE
{
	IMPLEMENT_CLASS_0(TextMesh);

	void TextMesh::init(Font* _font, const char* text)
	{
		setFont(_font);
		generateTextMesh(text);
	}

	void TextMesh::generateTextMesh(const char* text, bool bNormalizeHeight)
	{
		if (!m_bufferArray)
		{
			Handle hBufferData("Buffer Data", sizeof(BufferData));
			BufferData* pBufferData = new(hBufferData) BufferData(VERTEX_BUFFER);
			m_font->generateBufferDataForText(text, 1.0, pBufferData, bNormalizeHeight);
			Handle hBufferArray = BufferManager::getInstance()->createBufferArray(pBufferData, NULL, NULL);
			m_bufferArray = hBufferArray.getObject<IGPUBufferArray>();
		}
		else
		{
			IGPUBufferData* pVertexBufferData = m_bufferArray->getBufferAtIndex(0);
			BufferData* pBufferData = pVertexBufferData->getBufferData();
			m_font->generateBufferDataForText(text, 1.0, pBufferData, bNormalizeHeight);
			
			// Refresh buffer data and buffer array
			pVertexBufferData->FromBufferData(pBufferData);
			m_bufferArray->SetupBufferArray(pVertexBufferData, nullptr, nullptr);
		}
	}

	void TextMesh::setFont(Font* _font)
	{
		m_font = _font;
	}

}