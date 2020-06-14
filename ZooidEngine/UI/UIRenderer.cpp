#include "UIRenderer.h"
#include "ZooidUI.h"

#include "GameObjectModel/Component.h"

#include "Renderer/IRenderer.h"
#include "Renderer/RenderZooid.h"
#include "Resources/Texture.h"
#include "Renderer/IGPUTexture.h"
#include "Renderer/IGPUFrameBuffer.h"
#include "Renderer/IGPURenderBuffer.h"
#include "Renderer/IGPUBufferArray.h"
#include "Renderer/IGPUBufferData.h"
#include "Renderer/BufferData.h"
#include "Renderer/BufferLayout.h"
#include "Renderer/IGPUStates.h"

#include "ResourceManagers/ShaderManager.h"
#include "ResourceManagers/BufferManager.h"

#include "Events/Events.h"
#include "ZEGameContext.h"

namespace ZE
{
	class ZE_UIRenderer : public UIRenderer
	{
	public:
		ZE_UIRenderer();
		virtual ~ZE_UIRenderer() {}

		// UIRenderer Interface functions
		virtual void Init(int width, int height);
		virtual void ProcessCurrentDrawList();
		virtual void Destroy();
		virtual bool requestToClose();
		virtual void* getWindowContext();
		virtual UInt32 createRendererTexture(void* pAddress, UInt32 width, UInt32 height, UInt32 channelCount);
		virtual void destroyTexture(UInt32 textureHandle);
		virtual void destroyTextures();
		virtual void resizeWindow(int width, int height);
		//

	protected:
		void processDrawItem(UIDrawItem* drawItem);

		IGPUTexture* m_dynamicTexture;
		IGPUFrameBuffer* m_frameBuffer;
		IGPURenderBuffer* m_renderBuffer;

		IShaderChain* m_drawShader;
		IShaderChain* m_drawInstanceShader;
		IShaderChain* m_drawTexShader;
		IShaderChain* m_drawInstanceTexShader;
		IShaderChain* m_textShader;
		IShaderChain* m_textInstanceShader;
		
		BufferData* m_drawBuffer;
		IGPUBufferData* m_drawGPUBuffer;

		BufferData* m_instanceBuffer;
		IGPUBufferData* m_instanceGPUBuffer;

		IGPUBufferArray* m_instanceGPUBufferArray;
		IGPUBufferArray* m_instanceTextGPUBufferArray;
		IGPUBufferArray* m_drawBufferArray;

		Handle m_hDrawBufferArray;
		Handle m_hInstanceGPUBufferArray;
		Handle m_hInstanceTextGPUBufferArray;

		Array<Handle> textureHandles;
	};

	ZE_UIRenderer::ZE_UIRenderer()
	{}

	void ZE_UIRenderer::Init(int width, int height)
	{
		GameContext* gameContext = GetGameContext();

		ScopedRenderThreadOwnership renderLock(gameContext->getRenderer());

		// Setup Shaders
		m_drawShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/UI/BaseShapeShader.vs", "ZooidEngine/Shaders/UI/BaseShapeShader_Color.frag", nullptr, nullptr);
		m_drawInstanceShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/UI/BaseShapeShader_Instance.vs", "ZooidEngine/Shaders/UI/BaseShapeShader_Color_Instance.frag", nullptr, nullptr);
		m_drawTexShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/UI/BaseShapeShader.vs", "ZooidEngine/Shaders/UI/BaseShapeShader_Texture.frag", nullptr, nullptr);
		m_drawInstanceTexShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/UI/BaseShapeShader_Texture_Instance.vs", "ZooidEngine/Shaders/UI/BaseShapeShader_Texture_Instance.frag", nullptr, nullptr);
		m_textShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/UI/BaseTextShader.vs", "ZooidEngine/Shaders/UI/BaseTextShader.frag", nullptr, nullptr);
		m_textInstanceShader = ShaderManager::GetInstance()->makeShaderChain("ZooidEngine/Shaders/UI/BaseTextShader_Instance.vs", "ZooidEngine/Shaders/UI/BaseTextShader_Instance.frag", nullptr, nullptr);

		// Make Buffer Layout to match UI format
		UInt32 uiVertexLayout;
		BufferLayout* layout = BufferLayoutManager::GetInstance()->createAndAddBufferLayout(uiVertexLayout);
		layout->m_layouts.reset(3);
		layout->m_layouts.push_back({ 0, 3, EDataType::FLOAT, sizeof(UIVertex), 0, false });
		layout->m_layouts.push_back({ 1, 2, EDataType::FLOAT, sizeof(UIVertex), 3, false });
		layout->m_layouts.push_back({ 2, 4, EDataType::FLOAT, sizeof(UIVertex), 5, false });
		layout->calculateBufferDataCount();

		UInt32 uiVertexInstanceLayout;
		BufferLayout* instanceLayout = BufferLayoutManager::GetInstance()->createAndAddBufferLayout(uiVertexInstanceLayout);
		instanceLayout->m_layouts.reset(4);
		instanceLayout->m_layouts.push_back({ 0, 3, EDataType::FLOAT, sizeof(UIDrawInstance), 0, true }); // Position
		instanceLayout->m_layouts.push_back({ 1, 3, EDataType::FLOAT, sizeof(UIDrawInstance), 3, true }); // Dimension + Radius
		instanceLayout->m_layouts.push_back({ 2, 4, EDataType::FLOAT, sizeof(UIDrawInstance), 6, true }); // Color
		instanceLayout->m_layouts.push_back({ 3, 4, EDataType::FLOAT, sizeof(UIDrawInstance), 10, true }); // UV Coord + Dimension
		instanceLayout->calculateBufferDataCount();

		// Create buffer array for rect
		static UIVertex rectArray[6] = { 
									{ UIVector2{ 1.0f, 0.0f }, 0.0f, UIVector2{ 1.0f, 0.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 0.0f, 0.0f }, 0.0f, UIVector2{ 0.0f, 0.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 1.0f, 1.0f }, 0.0f, UIVector2{ 1.0f, 1.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 1.0f, 1.0f }, 0.0f, UIVector2{ 1.0f, 1.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 0.0f, 0.0f }, 0.0f, UIVector2{ 0.0f, 0.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 0.0f, 1.0f }, 0.0f, UIVector2{ 0.0f, 1.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } } 
									};

		Handle hRectBuffer("Rect Buffer", sizeof(BufferData));
		BufferData* rectBuffer = new(hRectBuffer) BufferData(VERTEX_BUFFER);
		rectBuffer->setBufferLayout(uiVertexLayout);
		rectBuffer->SetData(rectArray, sizeof(UIVertex), 6);

		Handle hInstanceBuffer("InstanceBuffer", sizeof(BufferData));
		m_instanceBuffer = new(hInstanceBuffer) BufferData(VERTEX_BUFFER);
		m_instanceBuffer->setBufferLayout(uiVertexInstanceLayout);
		m_instanceBuffer->SetData(nullptr, sizeof(UIDrawInstance), 0);
		m_instanceBuffer->setStaticBuffer(false);

		{			
			IGPUBufferData* gpuBuffers[2];
			gpuBuffers[0] = BufferManager::getInstance()->createGPUBufferFromBuffer(rectBuffer);
			m_instanceGPUBuffer = gpuBuffers[1] = BufferManager::getInstance()->createGPUBufferFromBuffer(m_instanceBuffer);

			m_hInstanceGPUBufferArray = gameContext->getRenderZooid()->CreateRenderBufferArray();
			if (m_hInstanceGPUBufferArray.isValid())
			{
				m_instanceGPUBufferArray = m_hInstanceGPUBufferArray.getObject<IGPUBufferArray>();
				m_instanceGPUBufferArray->SetupBufferArray(gpuBuffers, 2, nullptr, 0, nullptr, 0);
			}
		}

		// Create Buffer Array for rect Y-flip for Text
		static UIVertex rectTextArray[6] = { { UIVector2{ 0.0f, 0.0f }, 0.0f, UIVector2{ 0.0f, 1.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 1.0f, 0.0f }, 0.0f, UIVector2{ 1.0f, 1.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 1.0f, 1.0f }, 0.0f, UIVector2{ 1.0f, 0.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 0.0f, 0.0f }, 0.0f, UIVector2{ 0.0f, 1.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 1.0f, 1.0f }, 0.0f, UIVector2{ 1.0f, 0.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } },
									{ UIVector2{ 0.0f, 1.0f }, 0.0f, UIVector2{ 0.0f, 0.0f }, UIVector4{ 1.0f, 1.0f, 1.0f, 1.0f } } };

		Handle hTextRectBuffer("Rect Buffer", sizeof(BufferData));
		BufferData* textRectBuffer = new(hTextRectBuffer) BufferData(VERTEX_BUFFER);
		textRectBuffer->setBufferLayout(uiVertexLayout);
		textRectBuffer->SetData(rectTextArray, sizeof(UIVertex), 6);

		{
			IGPUBufferData* gpuBuffers[2];
			gpuBuffers[0] = BufferManager::getInstance()->createGPUBufferFromBuffer(textRectBuffer);
			gpuBuffers[1] = m_instanceGPUBuffer;

			m_hInstanceTextGPUBufferArray = gameContext->getRenderZooid()->CreateRenderBufferArray();
			if (m_hInstanceTextGPUBufferArray.isValid())
			{
				m_instanceTextGPUBufferArray = m_hInstanceTextGPUBufferArray.getObject<IGPUBufferArray>();
				m_instanceTextGPUBufferArray->SetupBufferArray(gpuBuffers, 2, nullptr, 0, nullptr, 0);
			}
		}

		Handle hDrawBuffer("Draw Buffer", sizeof(BufferData));
		m_drawBuffer = new(hDrawBuffer) BufferData(VERTEX_BUFFER);
		m_drawBuffer->setBufferLayout(uiVertexLayout);
		m_drawBuffer->SetData(nullptr, sizeof(UIVertex), 0);
		m_drawBuffer->setStaticBuffer(false);

		{
			IGPUBufferData* m_drawGPUBuffer = BufferManager::getInstance()->createGPUBufferFromBuffer(textRectBuffer);

			m_hDrawBufferArray = gameContext->getRenderZooid()->CreateRenderBufferArray();
			if (m_hDrawBufferArray.isValid())
			{
				m_drawBufferArray = m_hDrawBufferArray.getObject<IGPUBufferArray>();
				m_drawBufferArray->SetupBufferArray(m_drawGPUBuffer, nullptr, nullptr);
			}
		}
	}

	void ZE_UIRenderer::ProcessCurrentDrawList()
	{
		GameContext* gameContext = GetGameContext();
		IRenderer* renderer = gameContext->getRenderer();

		// Disable Depth Test

		renderer->SetRenderDepthStencilState(TRenderDepthStencilState< 
			false,
			false,
			true,
			ERendererCompareFunc::ALWAYS,
			ERendererCompareFunc::ALWAYS,
			0,
			0,
			0>::GetGPUState());
		
		// Enable Blend
		renderer->SetRenderBlendState(TRenderBlendState<true, ERendererBlendFactor::SRC_ALPHA, ERendererBlendFactor::ONE_MINUS_SRC_ALPHA>::GetGPUState());
		
		for (int i = 0; i < m_drawList->itemCount(); i++)
		{
			UIDrawItem* drawItem = m_drawList->getDrawItem(i);
			processDrawItem(drawItem);
		}

		// Reset Renderer Feature
		renderer->SetRenderBlendState(DefaultRenderBlendState::GetGPUState());
		renderer->SetRenderDepthStencilState(DefaultDepthStencilState::GetGPUState());
	}

	void ZE_UIRenderer::Destroy()
	{

	}

	bool ZE_UIRenderer::requestToClose()
	{
		return false;
	}

	void* ZE_UIRenderer::getWindowContext()
	{
		return nullptr;
	}

	ZE::UInt32 ZE_UIRenderer::createRendererTexture(void* pAddress, UInt32 width, UInt32 height, UInt32 channelCount)
	{
		GameContext* gameContext = GetGameContext();
		
		Handle hTexture("Texture", sizeof(Texture));
		Texture* texture = new(hTexture) Texture;
		texture->loadFromBuffer(pAddress, width, height, 1, channelCount);

		ScopedRenderThreadOwnership renderLock(gameContext->getRenderer());
		Handle hGPUTexture = gameContext->getRenderZooid()->CreateRenderTexture();
		if (hGPUTexture.isValid())
		{
			IGPUTexture* gpuTexture = hGPUTexture.getObject<IGPUTexture>();
			gpuTexture->fromTexture(texture);
			textureHandles.push_back(hGPUTexture);
			return textureHandles.length() - 1;
		}

		return 0;
	}

	void ZE_UIRenderer::destroyTexture(UInt32 textureHandle)
	{
		GameContext* gameContext = GetGameContext();
		
		Handle hGPUTexture = textureHandles[textureHandle];
		textureHandles.removeAt(textureHandle);
		if (hGPUTexture.isValid())
		{
			ScopedRenderThreadOwnership renderLock(gameContext->getRenderer());
			IGPUTexture* gpuTexture = hGPUTexture.getObject<IGPUTexture>();
			gpuTexture->release();
			hGPUTexture.release();
		}
	}

	void ZE_UIRenderer::destroyTextures()
	{
		GameContext* gameContext = GetGameContext();

		ScopedRenderThreadOwnership renderLock(gameContext->getRenderer());
		for (int i = 0; i < textureHandles.length(); i++)
		{
			Handle hGPUTexture = textureHandles[i];
			if (hGPUTexture.isValid())
			{
				IGPUTexture* gpuTexture = hGPUTexture.getObject<IGPUTexture>();
				gpuTexture->release();
				hGPUTexture.release();
			}
		}

		textureHandles.clear();
	}

	void ZE_UIRenderer::resizeWindow(int width, int height)
	{
		// Do Nothing
	}

	void ZE_UIRenderer::processDrawItem(UIDrawItem* drawItem)
	{
		IRenderer* mainRenderer = GetGameContext()->getRenderer();

		bool isFont = drawItem->getTextureHandle() && drawItem->isFont();
		bool isUsingRect = drawItem->isUsingRectInstance();
		
		IGPUBufferArray* bufferArray = isUsingRect ? m_instanceGPUBufferArray : m_drawBufferArray;
		IShaderChain* shader = m_drawShader;
		if (drawItem->getTextureHandle() > 0)
		{
			if (isFont)
			{
				bufferArray = isUsingRect ? m_instanceTextGPUBufferArray : m_drawBufferArray;
				shader = isUsingRect ? m_textInstanceShader : m_textShader;
				shader->bind();
			}
			else
			{
				shader = isUsingRect ? m_drawInstanceTexShader : m_drawTexShader;
				shader->bind();
			}
			Handle& hTexture = textureHandles[drawItem->getTextureHandle()];
			IGPUTexture* texture = hTexture.getObject<IGPUTexture>();
			shader->setTexture("InTexture", texture, 0);
			texture->bind();
		}
		else if (isUsingRect)
		{
			bufferArray = m_instanceGPUBufferArray;
			shader = m_drawInstanceShader;
			shader->bind();
		}
		else
		{
			shader->bind();
			shader->setFloat("roundness", drawItem->getRoundness());
			shader->setVec2("shapeDimension", Vector2(drawItem->getDimension().x, drawItem->getDimension().y));
		}

		shader->setVec2("screenDimension", Vector2(mainRenderer->GetWidth(), mainRenderer->GetHeight()));

		if (drawItem->getVertices().size() > 0)
		{
			m_drawBuffer->SetData(drawItem->getVertices().data(), sizeof(UIVertex), drawItem->getVertices().length());
			m_drawGPUBuffer->refresh();
		}

		if (drawItem->getInstances().size() > 0)
		{
			m_instanceBuffer->SetData(drawItem->getInstances().data(), sizeof(UIDrawInstance), drawItem->getInstances().length());
			m_instanceGPUBuffer->refresh();
		}

		if (isUsingRect)
		{
			// Draw Instance
			mainRenderer->DrawBufferArrayInstanced(ERenderTopologyEnum::TOPOLOGY_TRIANGLE, bufferArray, 6, 0, drawItem->getInstances().size());
		}
		else
		{
			// Draw Vertex
			mainRenderer->DrawBufferArray(ERenderTopologyEnum::TOPOLOGY_TRIANGLE, bufferArray, drawItem->getVertices().length());
		}

		shader->unbind();
	}

	ZE::UIRenderer* UI::Platform::CreateRenderer()
	{
		return UINEW(ZE_UIRenderer);
	}
}