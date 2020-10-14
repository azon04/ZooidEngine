#include "TextMesh.h"

#include "Font.h"
#include "ZEGameContext.h"
#include "Renderer/IRenderer.h"
#include "Renderer/DrawList.h"
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

	void TextMesh::generateTextMesh(const char* text, bool bNormalizeHeight, DrawList* _drawList)
	{
		if (!m_bufferArray)
		{
			ScopedRenderThreadOwnership renderLock(gGameContext->getRenderer());

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
			
			if (_drawList)
			{
				_drawList->m_commandList.registerCommand([](void* data) {

					if (!data) { return; }

					IGPUBufferArray* bufferArray = reinterpret_cast<IGPUBufferArray*>(data);
					IGPUBufferData* pVertexBufferData = bufferArray->getBufferAtIndex(0);
					BufferData* pBufferData = pVertexBufferData->getBufferData();

					// Refresh buffer data and buffer array
					pVertexBufferData->FromBufferData(pBufferData);
					bufferArray->SetupBufferArray(pVertexBufferData, nullptr, nullptr);
				}, m_bufferArray);
			}
			else
			{

				ScopedRenderThreadOwnership renderLock(gGameContext->getRenderer());
				
				// Refresh buffer data and buffer array
				pVertexBufferData->FromBufferData(pBufferData);
				m_bufferArray->SetupBufferArray(pVertexBufferData, nullptr, nullptr);
			}
		}
	}

	void TextMesh::setFont(Font* _font)
	{
		m_font = _font;
	}

}