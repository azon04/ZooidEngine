#include "TextComponent.h"
#include "ZEGameContext.h"

#include "Events/Events.h"

#include "Math/MathOps.h"

#include "Renderer/IRenderer.h"
#include "Renderer/IGPUBufferArray.h"

#include "Renderer/DrawList.h"

#include "ResourceManagers/FontManager.h"
#include "ResourceManagers/ShaderManager.h"
#include "Resources/Font.h"
#include "Resources/TextMesh.h"

#include "SceneRenderer/RenderInfo/TextRenderInfo.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(TextComponent, SceneComponent)

	void TextComponent::setupComponent()
	{
		SceneComponent::setupComponent();

		addEventDelegate(Event_GATHER_RENDER, &TextComponent::handleGatherRender);
		
		if (!m_textMesh)
		{
			ScopedRenderThreadOwnership renderLock(m_gameContext->getRenderer());

			Handle hTextMesh("TextMesh", sizeof(TextMesh));
			m_textMesh = new(hTextMesh) TextMesh();
			if (!m_font)
			{
				m_font = FontManager::GetDefaultFont();
			}
			m_textMesh->init(m_font, m_text.const_str());
		}
	}

	void TextComponent::handleGatherRender(Event* evt)
	{
		if (!m_bVisible) { return; }

		if (m_bDirty)
		{
			ScopedRenderThreadOwnership renderLock(m_gameContext->getRenderer());
			m_textMesh->generateTextMesh(m_text.const_str(), m_drawSpace == DRAW_WORLD_SPACE);
			m_bDirty = false;
		}

		if (m_textMesh)
		{
			if (m_drawSpace == DRAW_SCREEN)
			{
				TextRenderInfo* textRenderInfo = m_gameContext->getDrawList()->m_textScreenRenderGatherer.nextRenderInfo();

				textRenderInfo->m_gpuBufferArray = m_textMesh->getGPUBufferArray();
				textRenderInfo->m_renderTopology = TOPOLOGY_TRIANGLE;

				int shaderType = Z_SHADER_CHAIN_SCREEN_TEXT_SHADER;
				switch (m_font->getFontRenderMethod())
				{
				case FONT_RENDER_SDF:
				case FONT_RENDER_PSDF:
					shaderType = Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_SDF;
					break;
				case FONT_RENDER_MSDF:
					shaderType = Z_SHADER_CHAIN_SCREEN_TEXT_SHADER_MSDF;
					break;
				default:
					break;
				}

				textRenderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(shaderType);
				textRenderInfo->m_worldTransform = m_worldTransform;
				textRenderInfo->m_color = m_color;
				textRenderInfo->m_fontTexture = m_font->getGPUTexture();
				textRenderInfo->drawCount = m_textMesh->getGPUBufferArray()->getDataCount();
			}
			else
			{
				TextRenderInfo* textRenderInfo = m_gameContext->getDrawList()->m_textSceneRenderGatherer.nextRenderInfo();

				textRenderInfo->m_gpuBufferArray = m_textMesh->getGPUBufferArray();
				textRenderInfo->m_renderTopology = TOPOLOGY_TRIANGLE;

				int shaderType = Z_SHADER_CHAIN_WORLD_TEXT_SHADER;
				switch (m_font->getFontRenderMethod())
				{
				case FONT_RENDER_SDF:
				case FONT_RENDER_PSDF:
					shaderType = Z_SHADER_CHAIN_WORLD_TEXT_SHADER_SDF;
					break;
				case FONT_RENDER_MSDF:
					shaderType = Z_SHADER_CHAIN_WORLD_TEXT_SHADER_MSDF;
					break;
				default:
					break;
				}

				textRenderInfo->m_shaderChain = ShaderManager::GetInstance()->getShaderChain(shaderType);
				textRenderInfo->m_worldTransform = m_worldTransform;
				textRenderInfo->m_color = m_color;
				textRenderInfo->m_fontTexture = m_font->getGPUTexture();
				textRenderInfo->drawCount = m_textMesh->getGPUBufferArray()->getDataCount();
			}
		}
	}

	void TextComponent::setText(String text)
	{
		m_bDirty = text != m_text;
		if (m_bDirty)
		{
			m_text = text;
		}
	}

}