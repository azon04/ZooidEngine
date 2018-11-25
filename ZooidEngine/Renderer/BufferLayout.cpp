#include "BufferLayout.h"

#include <GL/glew.h>

namespace ZE 
{

	BufferLayoutManager* BufferLayoutManager::m_instance = nullptr;

	void BufferLayoutManager::Init()
	{
		m_instance = new BufferLayoutManager;
		m_instance->InitLayout();
	}

	ZE::BufferLayoutManager* BufferLayoutManager::GetInstance()
	{
		return m_instance;
	}

	void BufferLayoutManager::Destroy()
	{
		m_instance->DestroyLayout();
		delete m_instance;
	}

	void BufferLayoutManager::InitLayout()
	{
		m_bufferLayout.reset(6);

		{
			// Vertex : { Position, Color }
			Handle h("Buffer Layout", sizeof(BufferLayout));
			BufferLayout* vertexColorLayout = new(h) BufferLayout();
			vertexColorLayout->m_layouts.reset(2);
			vertexColorLayout->m_layouts.push_back({ 0, 3, EDataType::FLOAT, 6 * sizeof(float), 0, false });
			vertexColorLayout->m_layouts.push_back({ 1, 3, EDataType::FLOAT, 6 * sizeof(float), 3, false });
			vertexColorLayout->calculateBufferDataCount();

			m_bufferLayout.push_back(vertexColorLayout);
		}

		{
			// Vertex : { Postion, TexCoord }
			Handle h("Buffer Layout", sizeof(BufferLayout));
			BufferLayout* vertexTexCoord = new(h) BufferLayout();
			vertexTexCoord->m_layouts.reset(2);
			vertexTexCoord->m_layouts.push_back({ 0, 3, EDataType::FLOAT, 5 * sizeof(float), 0, false });
			vertexTexCoord->m_layouts.push_back({ 1, 3, EDataType::FLOAT, 5 * sizeof(float), 3, false });
			vertexTexCoord->calculateBufferDataCount();

			m_bufferLayout.push_back(vertexTexCoord);
		}

		{
			// Vertex : { Postion, Color, TexCoord }
			Handle h("Buffer Layout", sizeof(BufferLayout));
			BufferLayout* texCoordLayout = new(h) BufferLayout();
			texCoordLayout->m_layouts.reset(3);
			texCoordLayout->m_layouts.push_back({ 0, 3, EDataType::FLOAT, 8 * sizeof(float), 0, false });
			texCoordLayout->m_layouts.push_back({ 1, 3, EDataType::FLOAT, 8 * sizeof(float), 3, false });
			texCoordLayout->m_layouts.push_back({ 2, 2, EDataType::FLOAT, 8 * sizeof(float), 6, false });
			texCoordLayout->calculateBufferDataCount();

			m_bufferLayout.push_back(texCoordLayout);
		}

		{
			// Vertex : { Postion, Normal, TexCoord }

			Handle h("Buffer Layout", sizeof(BufferLayout));
			BufferLayout* normalTexCoordLayout = new(h) BufferLayout();
			normalTexCoordLayout->m_layouts.reset(3);
			normalTexCoordLayout->m_layouts.push_back({ 0, 3, EDataType::FLOAT, 8 * sizeof(float), 0, false });
			normalTexCoordLayout->m_layouts.push_back({ 1, 3, EDataType::FLOAT, 8 * sizeof(float), 3, false });
			normalTexCoordLayout->m_layouts.push_back({ 2, 2, EDataType::FLOAT, 8 * sizeof(float), 6, false });
			normalTexCoordLayout->calculateBufferDataCount();

			m_bufferLayout.push_back(normalTexCoordLayout);
		}

		{
			// Vertex : { Postion, Normal, TexCoord, BoneIDs, BoneWeights }

			Handle h("Buffer Layout", sizeof(BufferLayout));
			BufferLayout* normalTexCoordLayout = new(h) BufferLayout();
			normalTexCoordLayout->m_layouts.reset(5);
			UInt32 stride = 12 * sizeof(float) + 4 * sizeof(Int32);
			normalTexCoordLayout->m_layouts.push_back({ 0, 3, EDataType::FLOAT, stride, 0, false });
			normalTexCoordLayout->m_layouts.push_back({ 1, 3, EDataType::FLOAT, stride, 3, false });
			normalTexCoordLayout->m_layouts.push_back({ 2, 2, EDataType::FLOAT, stride, 6, false });
			normalTexCoordLayout->m_layouts.push_back({ 3, 4, EDataType::INTEGER, stride, 8, false });
			normalTexCoordLayout->m_layouts.push_back({ 4, 4, EDataType::FLOAT, stride, 12, false });
			normalTexCoordLayout->calculateBufferDataCount();

			m_bufferLayout.push_back(normalTexCoordLayout);
		}

		{
			// Vertex : { Vector4 }

			Handle h("Buffer Layout", sizeof(BufferLayout));
			BufferLayout* normalTexCoordLayout = new(h) BufferLayout();
			normalTexCoordLayout->m_layouts.reset(1);
			normalTexCoordLayout->m_layouts.push_back({ 0, 4, EDataType::FLOAT, 4 * sizeof(float), 0, false });
			normalTexCoordLayout->calculateBufferDataCount();

			m_bufferLayout.push_back(normalTexCoordLayout);
		}
	}

	void BufferLayoutManager::DestroyLayout()
	{
		m_bufferLayout.clear();
	}

	ZE::BufferLayout* BufferLayoutManager::getBufferLayoutByFormat(int format)
	{
		return m_bufferLayout[format];
	}

	ZE::UInt32 BufferLayoutManager::addNewBufferLayout(BufferLayout* bufferLayout)
	{
		m_bufferLayout.push_back(bufferLayout);
		return m_bufferLayout.size() - 1;
	}

	ZE::BufferLayout* BufferLayoutManager::createAndAddBufferLayout(UInt32& bufferIndex)
	{
		Handle h("Buffer Layout", sizeof(BufferLayout));
		BufferLayout* bufferLayout = new(h) BufferLayout;
		bufferIndex = addNewBufferLayout(bufferLayout);
		return bufferLayout;
	}

	ZE::UInt32 BufferLayout::calculateBufferDataCount()
	{
		m_bufferDataCount = 0;
		for (int i = 0; i < m_layouts.length(); i++)
		{
			m_bufferDataCount += m_layouts[i].SizePerItem;
		}
		return m_bufferDataCount;
	}

}