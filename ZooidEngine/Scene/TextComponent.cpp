#include "TextComponent.h"
#include "ZEGameContext.h"

#include "Events/Events.h"

#include "Math/MathOps.h"

#include "Renderer/IRenderer.h"
#include "Renderer/DrawList.h"

#include "ResourceManagers/FontManager.h"
#include "ResourceManagers/ShaderManager.h"
#include "Resources/Font.h"
#include "Resources/TextMesh.h"

namespace ZE
{
	IMPLEMENT_CLASS_1(TextComponent, SceneComponent)

	void TextComponent::setupComponent()
	{
		SceneComponent::setupComponent();

		addEventDelegate(Event_GATHER_RENDER, &TextComponent::handleGatherRender);
		
		if (!m_textMesh)
		{
			m_gameContext->getRenderer()->AcquireRenderThreadOwnership();

			Handle hTextMesh("TextMesh", sizeof(TextMesh));
			m_textMesh = new(hTextMesh) TextMesh();
			if (!m_font)
			{
				m_font = FontManager::GetDefaultFont();
			}
			m_textMesh->init(m_font, m_text.const_str());
			m_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
		}
	}

	void TextComponent::handleGatherRender(Event* evt)
	{
		if (!m_bVisible) { return; }

		if (m_bDirty)
		{
			m_gameContext->getRenderer()->AcquireRenderThreadOwnership();
			m_textMesh->generateTextMesh(m_text.const_str(), m_drawSpace == DRAW_WORLD_SPACE);
			m_gameContext->getRenderer()->ReleaseRenderThreadOwnership();
			m_bDirty = false;
		}

		if (m_textMesh)
		{
			if (m_drawSpace == DRAW_SCREEN)
			{
				ShaderAction& shaderAction = m_gameContext->getDrawList()->getNextSecondPassShaderAction();
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
				shaderAction.setShaderAndBuffer(ShaderManager::GetInstance()->getShaderChain(shaderType), m_textMesh->getGPUBufferArray());
				shaderAction.setShaderFloatVar("screenHeight", m_gameContext->getRenderer()->GetHeight());
				shaderAction.setShaderFloatVar("screenWidth", m_gameContext->getRenderer()->GetWidth());
				shaderAction.setShaderMatVar("model", m_worldTransform);

				shaderAction.setShaderTextureVar("fontTexture", m_font->getGPUTexture(), 0);
				shaderAction.setShaderVec3Var("color", m_color);
				EnableAndSetBlendFunc(shaderAction, ERendererBlendFactor::SRC_ALPHA, ERendererBlendFactor::ONE_MINUS_SRC_ALPHA);
			}
			else
			{
				ShaderAction& shaderAction = m_gameContext->getDrawList()->getNextSecondPassShaderAction();

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

				shaderAction.setShaderAndBuffer(ShaderManager::GetInstance()->getShaderChain(shaderType), m_textMesh->getGPUBufferArray());
				shaderAction.setShaderMatVar("modelMat", m_worldTransform);

				shaderAction.setShaderTextureVar("fontTexture", m_font->getGPUTexture(), 0);
				shaderAction.setShaderVec3Var("color", m_color);
				EnableAndSetBlendFunc(shaderAction, ERendererBlendFactor::SRC_ALPHA, ERendererBlendFactor::ONE_MINUS_SRC_ALPHA);
				shaderAction.addShaderFeature(FACE_CULING, false);
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