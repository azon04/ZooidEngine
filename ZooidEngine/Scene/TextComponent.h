#ifndef __ZE_TEXT_COMPONENT_H__
#define __ZE_TEXT_COMPONENT_H__

#include "SceneComponent.h"
#include "Utils/String.h"

namespace ZE
{
	class TextMesh;
	class Event;
	class GameContext;
	class Font;
	enum DrawSpace : UInt8
	{
		DRAW_SCREEN,
		DRAW_WORLD_SPACE
	};

	class TextComponent : public SceneComponent
	{
		DEFINE_CLASS(TextComponent)

	public:

		TextComponent(GameContext* _gameContext) :
			SceneComponent(_gameContext),
			m_textMesh(NULL),
			m_font(NULL),
			m_text(""),
			m_bVisible(true),
			m_drawSpace(DRAW_SCREEN)
			{}

		virtual void setupComponent() override;
		void handleGatherRender(Event* evt);

		void setText(String text);
		String getText() const { return m_text; }
		
		void setVisible(bool _bVisible) {
			m_bVisible = _bVisible;
		}
		bool isVisible() const { return m_bVisible; }

		void setColor(Vector3 _color) { m_color = _color; }
		void setDrawSpace(DrawSpace _drawSpace) 
		{ 
			m_drawSpace = _drawSpace; 
			m_bDirty = true; 
		}

	protected:
		TextMesh* m_textMesh;
		Font* m_font;
		String m_text;
		bool m_bDirty;
		bool m_bVisible;
		DrawSpace m_drawSpace;
		Vector3 m_color;
	};
}
#endif
