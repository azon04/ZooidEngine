#include "ZooidUI.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <stdlib.h>
#include <iostream>
#include "UIRenderer.h"

#include "External/Image/stb_image.h"

#define UIMEM_CPY(Dst, Src, _Size) memcpy(Dst, Src, _Size)
#define UIMEM_ZERO(Dst, _Size) memset(Dst, 0, _Size)
#define UIMAX(a, b) (a > b ? (a) : (b))
#define UIMIN(a, b) (a < b ? (a) : (b))

#define MERGE_PATH(p1, p2) p1 p2

namespace ZE
{
	UIState UI::MainUIState;
	UIButtonStyle UI::DefaultButtonStyle;
	UIButtonStyle UI::DefaultCheckBoxStyle;
	UIButtonStyle UI::DefaultRadioBtnStyle;
	UISliderStyle UI::DefaultSliderStyle;
	UIPanelStyle UI::DefaultPanelStyle;
	UIFont* UI::DefaultFont = nullptr;

	ZE::UIState* UI::GetUIState()
	{
		return &MainUIState;
	}

	void UI::Init(Int32 width, Int32 height)
	{
		MainUIState.drawer = UINEW(UIDrawer);
		MainUIState.renderer = Platform::CreateRenderer();

		MainUIState.renderer->Init(width, height);

		UITextureManager::Init();

		// Default Textures
		ZE::UITexture* checkBoxBg = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER,"Textures/CheckBox_Bg.png"), MainUIState.renderer);
		ZE::UITexture* checkBoxChecked = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/CheckBox_Checked.png"), MainUIState.renderer);
		ZE::UITexture* radioBtnBg = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/RadioBtn_Bg.png"), MainUIState.renderer);
		ZE::UITexture* radioBtnChecked = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/RadioBtn_Checked.png"), MainUIState.renderer);
		ZE::UITexture* panelOpenIcon = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/PanelOpen.png"), MainUIState.renderer);
		ZE::UITexture* panelClosedIcon = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/PanelClosed.png"), MainUIState.renderer);
		ZE::UITexture* panelBg = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/PanelBgSmall_Style.png"), MainUIState.renderer);
		ZE::UITexture* panelClosedBg = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/PanelBgSmallClosed_Style.png"), MainUIState.renderer);
		ZE::UITexture* ButtonBgUp = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/ButtonBg_Up.png"), MainUIState.renderer);
		ZE::UITexture* ButtonBgHovered = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/ButtonBg_Hovered.png"), MainUIState.renderer);
		ZE::UITexture* ButtonBgDown = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/ButtonBg_Down.png"), MainUIState.renderer);

		// Default Font
		DefaultFont = ZE::UIFont::loadFontFile(MERGE_PATH(RESOURCE_FOLDER, DEFAULT_FONT_PATH), MainUIState.renderer, 16);

		// Button Default
		DefaultButtonStyle.up.texture = ButtonBgUp;
		DefaultButtonStyle.up.textureScale = SCALE_9SCALE;
		DefaultButtonStyle.up.textureOffset = { 0.35f, 0.35f, 0.35f, 0.35f };
		DefaultButtonStyle.up.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DefaultButtonStyle.hover.texture = ButtonBgHovered;
		DefaultButtonStyle.hover.textureScale = SCALE_9SCALE;
		DefaultButtonStyle.hover.textureOffset = { 0.35f, 0.35f, 0.35f, 0.35f };
		DefaultButtonStyle.hover.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DefaultButtonStyle.down.texture = ButtonBgDown;
		DefaultButtonStyle.down.textureScale = SCALE_9SCALE;
		DefaultButtonStyle.down.textureOffset = { 0.35f, 0.35f, 0.35f, 0.35f };
		DefaultButtonStyle.down.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Check Box Default
		DefaultCheckBoxStyle.up.texture = checkBoxBg;
		DefaultCheckBoxStyle.up.textureSize = { 16, 16 };
		DefaultCheckBoxStyle.up.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DefaultCheckBoxStyle.down.texture = checkBoxBg;
		DefaultCheckBoxStyle.down.textureSize = { 16, 16 };
		DefaultCheckBoxStyle.down.fillColor = { 0.75f, 0.75f, 0.75f, 1.0f };
		DefaultCheckBoxStyle.hover.texture = checkBoxBg;
		DefaultCheckBoxStyle.hover.textureSize = { 16, 16 };
		DefaultCheckBoxStyle.hover.fillColor = { 0.75f, 0.75f, 0.75f, 1.0f };
		DefaultCheckBoxStyle.selected.texture = checkBoxChecked;
		DefaultCheckBoxStyle.selected.textureSize = { 16, 16 };
		DefaultCheckBoxStyle.selected.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Default Radio Button Style
		DefaultRadioBtnStyle.up.texture = radioBtnBg;
		DefaultRadioBtnStyle.up.textureSize = { 16, 16 };
		DefaultRadioBtnStyle.up.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DefaultRadioBtnStyle.down.texture = radioBtnBg;
		DefaultRadioBtnStyle.down.textureSize = { 16, 16 };
		DefaultRadioBtnStyle.down.fillColor = { 0.75f, 0.75f, 0.75f, 1.0f };
		DefaultRadioBtnStyle.hover.texture = radioBtnBg;
		DefaultRadioBtnStyle.hover.textureSize = { 16, 16 };
		DefaultRadioBtnStyle.hover.fillColor = { 0.75f, 0.75f, 0.75f, 1.0f };
		DefaultRadioBtnStyle.selected.texture = radioBtnChecked;
		DefaultRadioBtnStyle.selected.textureSize = { 16, 16 };
		DefaultRadioBtnStyle.selected.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };

		// Default Slider Style
		DefaultSliderStyle.slider.fillColor = { 0.25f, 0.45f, 0.6f, 1.0f } ;
		DefaultSliderStyle.slider.texture = nullptr;
		DefaultSliderStyle.sliderButton = ZE::UI::DefaultButtonStyle;
		DefaultSliderStyle.sliderButton.up.fillColor = { 0.5f, 0.75f, 1.0f, 1.0f };
		DefaultSliderStyle.sliderButton.hover.fillColor = { 0.5f, 0.75f, 1.0f, 0.75f };
		DefaultSliderStyle.sliderButton.down.fillColor = { 0.5f, 0.75f, 1.0f, 0.5f };

		// Default Panel Style
		DefaultPanelStyle.headerHeight = 20.0f;
		DefaultPanelStyle.panel.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DefaultPanelStyle.panel.texture = panelBg;
		DefaultPanelStyle.panel.textureScale = SCALE_9SCALE;
		DefaultPanelStyle.panel.textureOffset = { 0.40f, 0.15f, 0.15f, 0.15f };
		DefaultPanelStyle.panelClosedBG.fillColor = { 1.0f, 1.0f, 1.0f, 1.0f };
		DefaultPanelStyle.panelClosedBG.texture = panelClosedBg;
		DefaultPanelStyle.panelClosedBG.textureScale = SCALE_9SCALE;
		DefaultPanelStyle.panelClosedBG.textureOffset = { 0.15f, 0.15f, 0.15f, 0.15f };
		DefaultPanelStyle.panelClosed.texture = panelClosedIcon;
		DefaultPanelStyle.panelClosed.textureSize = { 15,15 };
		DefaultPanelStyle.panelClosed.fillColor = { 1.0f ,1.0f, 1.0f, 1.0f };
		DefaultPanelStyle.panelOpened.texture = panelOpenIcon;
		DefaultPanelStyle.panelOpened.textureSize = { 15,15 };
		DefaultPanelStyle.panelOpened.fillColor = { 1.0f, 1.0f , 1.0f ,1.0f };
	}

	void UI::Destroy()
	{
		UITextureManager::Destroy();

		if (MainUIState.renderer)
		{
			MainUIState.renderer->Destroy();
			UIFREE(MainUIState.renderer);
		}

		if (MainUIState.drawer)
		{
			UIFREE(MainUIState.drawer);
		}
	}

	void UI::BeginFrame()
	{
		MainUIState.drawer->Reset();
	}

	void UI::EndFrame()
	{
		MainUIState.renderer->setDrawList(MainUIState.drawer->getCurrentDrawList());
		MainUIState.drawer->SwapBuffer();
	}

	void UI::UpdateMouseState(Float32 mouseX, Float32 mouseY, EButtonState mouseDown)
	{
		MainUIState.mouseDeltaX = mouseX - MainUIState.mouseX;
		MainUIState.mouseDeltaY = mouseY - MainUIState.mouseY;
		MainUIState.mouseX = mouseX;
		MainUIState.mouseY = mouseY;
		MainUIState.mouseDown = mouseDown;
	}
	
	bool UI::DoButton(Int32 _id, UIRect& rect, const UIButtonStyle& buttonStyle)
	{
		bool mouseInside = rect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });
		bool bPressed = false;
		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if ( mouseInside )
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; bPressed = true; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = -1; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}

		UIStyle style;
		if (MainUIState.activeItem.id == _id)
		{
			style = buttonStyle.down;
		}
		else if(MainUIState.hotItem.id == _id)
		{
			style = buttonStyle.hover;
		}
		else
		{
			style = buttonStyle.up;
		}

		if (style.texture)
		{
			MainUIState.drawer->DrawTexture(rect, style.texture, style.fillColor, style.textureScale, style.textureOffset);
		}
		else
		{
			MainUIState.drawer->DrawRect(rect, style.fillColor);
		}
		return bPressed;
	}

	bool UI::DoCheckBox(Int32 _id, const UIVector2& pos, const UIChar* text, bool bChecked, const UIButtonStyle& checkBoxStyle)
	{
		UIFont* font = checkBoxStyle.fontStyle.font ? checkBoxStyle.fontStyle.font : DefaultFont;
		UIRect rect;
		rect.m_pos = pos;
		rect.m_dimension.x = 0;
		rect.m_dimension.y = 0;

		UIStyle style;

		if (MainUIState.activeItem.id == _id)
		{
			style = checkBoxStyle.down;
		}
		else if (MainUIState.hotItem.id == _id)
		{
			style = checkBoxStyle.hover;
		}
		else
		{
			style = checkBoxStyle.up;
		}

		rect.m_dimension.x = (style.texture ? style.textureSize.x : 0) + font->calculateTextLength(text, 1.0f);
		rect.m_dimension.y = UIMAX((style.texture ? style.textureSize.y : 0), font->calculateTextHeight(1.0f));

		bool mouseInside = rect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });
		bool bPressed = false;
		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside)
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; bPressed = true; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = -1; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}

		if (bPressed)
		{
			bChecked = !bChecked;
		}

		Float32 textOffsetX = 0;
		if (!bChecked)
		{
			if (MainUIState.activeItem.id == _id)
			{
				if (checkBoxStyle.down.texture)
				{
					UIRect drawRect;
					drawRect.m_pos = pos;
					drawRect.m_dimension = checkBoxStyle.down.textureSize;
					textOffsetX = drawRect.m_dimension.x;
					MainUIState.drawer->DrawTexture(drawRect, checkBoxStyle.down.texture, checkBoxStyle.down.fillColor);
				}
			}
			else if (MainUIState.hotItem.id == _id)
			{
				if (checkBoxStyle.hover.texture)
				{
					UIRect drawRect;
					drawRect.m_pos = pos;
					drawRect.m_dimension = checkBoxStyle.hover.textureSize;
					textOffsetX = drawRect.m_dimension.x;
					MainUIState.drawer->DrawTexture(drawRect, checkBoxStyle.hover.texture, checkBoxStyle.hover.fillColor);
				}
			}
			else
			{
				if (checkBoxStyle.up.texture)
				{
					UIRect drawRect;
					drawRect.m_pos = pos;
					drawRect.m_dimension = checkBoxStyle.up.textureSize;
					textOffsetX = drawRect.m_dimension.x;
					MainUIState.drawer->DrawTexture(drawRect, checkBoxStyle.up.texture, checkBoxStyle.up.fillColor);
				}
			}
		}
		else
		{
			if (checkBoxStyle.selected.texture)
			{
				UIRect drawRect;
				drawRect.m_pos = pos;
				drawRect.m_dimension = checkBoxStyle.selected.textureSize;
				textOffsetX = drawRect.m_dimension.x;
				MainUIState.drawer->DrawTexture(drawRect, checkBoxStyle.selected.texture, checkBoxStyle.selected.fillColor);
			}
		}

		if (text[0] != '\0')
		{
			MainUIState.drawer->DrawText(UIVector2{ pos.x + textOffsetX + 5, pos.y + (rect.m_dimension.y - font->calculateTextHeight(1.0f)) / 2 }, UIVector4(1.0f), font, text, checkBoxStyle.fontStyle.fontScale);
		}

		return bChecked;
	}

	ZE::Int32 UI::DoRadioButton(Int32 _id, const UIVector2& pos, const UIChar* text, Int32 _selectedId, const UIButtonStyle& radioButtonStyle)
	{
		bool bChecked = _id == _selectedId;

		UIFont* font = radioButtonStyle.fontStyle.font ? radioButtonStyle.fontStyle.font : DefaultFont;

		UIRect rect;
		rect.m_pos = pos;
		rect.m_dimension.x = 0;
		rect.m_dimension.y = 0;

		UIStyle style;

		if (MainUIState.activeItem.id == _id)
		{
			style = radioButtonStyle.down;
		}
		else if (MainUIState.hotItem.id == _id)
		{
			style = radioButtonStyle.hover;
		}
		else
		{
			style = radioButtonStyle.up;
		}

		rect.m_dimension.x = (style.texture ? style.textureSize.x : 0) + font->calculateTextLength(text, 1.0f);
		rect.m_dimension.y = UIMAX((style.texture ? style.textureSize.y : 0), font->calculateTextHeight(1.0f));

		bool mouseInside = rect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });
		bool bPressed = false;
		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside)
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; bPressed = true; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = -1; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}

		if (bPressed)
		{
			_selectedId = _id;
		}

		Float32 textOffsetX = 0;
		if (!bChecked)
		{
			if (MainUIState.activeItem.id == _id)
			{
				if (radioButtonStyle.down.texture)
				{
					UIRect drawRect;
					drawRect.m_pos = pos;
					drawRect.m_dimension = radioButtonStyle.down.textureSize;
					textOffsetX = drawRect.m_dimension.x;
					MainUIState.drawer->DrawTexture(drawRect, radioButtonStyle.down.texture, radioButtonStyle.down.fillColor);
				}
			}
			else if (MainUIState.hotItem.id == _id)
			{
				if (radioButtonStyle.hover.texture)
				{
					UIRect drawRect;
					drawRect.m_pos = pos;
					drawRect.m_dimension = radioButtonStyle.hover.textureSize;
					textOffsetX = drawRect.m_dimension.x;
					MainUIState.drawer->DrawTexture(drawRect, radioButtonStyle.hover.texture, radioButtonStyle.hover.fillColor);
				}
			}
			else
			{
				if (radioButtonStyle.up.texture)
				{
					UIRect drawRect;
					drawRect.m_pos = pos;
					drawRect.m_dimension = radioButtonStyle.up.textureSize;
					textOffsetX = drawRect.m_dimension.x;
					MainUIState.drawer->DrawTexture(drawRect, radioButtonStyle.up.texture, radioButtonStyle.up.fillColor);
				}
			}
		}
		else
		{
			if (radioButtonStyle.selected.texture)
			{
				UIRect drawRect;
				drawRect.m_pos = pos;
				drawRect.m_dimension = radioButtonStyle.selected.textureSize;
				textOffsetX = drawRect.m_dimension.x;
				MainUIState.drawer->DrawTexture(drawRect, radioButtonStyle.selected.texture, radioButtonStyle.selected.fillColor);
			}
		}

		if (text[0] != '\0')
		{
			MainUIState.drawer->DrawText(UIVector2{ pos.x + textOffsetX + 5, pos.y + (rect.m_dimension.y - font->calculateTextHeight(1.0f)) / 2 }, UIVector4(1.0f), font, text);
		}

		return _selectedId;
	}

	void UI::DoPanel(Int32 _id, const UIRect& panelRect, const UIChar* text, Float32 padding, UIVector2& contentPos, bool& bClosed, const UIPanelStyle& style)
	{
		UIFont* font = style.headerFontStyle.font ? style.headerFontStyle.font : DefaultFont;
		Float32 xOffset = padding;
		Float32 headerHeight = style.headerHeight;

		if (bClosed)
		{
			if (style.panelClosed.texture)
			{
				headerHeight = UIMAX(headerHeight, style.panelClosed.textureSize.y);
			}
		}
		else
		{
			if (style.panelOpened.texture)
			{
				headerHeight = UIMAX(headerHeight, style.panelOpened.textureSize.y);
			}
		}

		if (!bClosed)
		{
			if (style.panel.texture)
			{
				MainUIState.drawer->DrawTexture(panelRect, style.panel.texture, style.panel.fillColor, style.panel.textureScale, style.panel.textureOffset);
			}
			else
			{
				MainUIState.drawer->DrawRect(panelRect, style.panel.fillColor);
			}
		}
		else
		{
			UIRect closeRect;
			closeRect.m_pos = panelRect.m_pos;
			closeRect.m_dimension = { panelRect.m_dimension.x, headerHeight + 5.0f };
			closeRect.m_roundness = panelRect.m_roundness;
			if (style.panelClosedBG.texture)
			{
				MainUIState.drawer->DrawTexture(closeRect, style.panelClosedBG.texture, style.panelClosedBG.fillColor, style.panelClosedBG.textureScale, style.panelClosedBG.textureOffset);
			}
			else
			{
				MainUIState.drawer->DrawRect(closeRect, style.panelClosedBG.fillColor);
			}
		}


		UIRect iconRect;
		if (bClosed)
		{
			if (style.panelClosed.texture)
			{
				iconRect.m_pos = panelRect.m_pos + UIVector2{ xOffset, (headerHeight + 0.5f * padding - style.panelClosed.textureSize.y) / 2 };
				iconRect.m_dimension = style.panelClosed.textureSize;
				MainUIState.drawer->DrawTexture(iconRect, style.panelClosed.texture, style.panelClosed.fillColor);
				xOffset += iconRect.m_dimension.x + 5;
			}
		}
		else
		{
			if (style.panelOpened.texture)
			{
				iconRect.m_pos = panelRect.m_pos + UIVector2{ xOffset, (headerHeight + 0.5f * padding - style.panelOpened.textureSize.y) / 2 };
				iconRect.m_dimension = style.panelOpened.textureSize;
				MainUIState.drawer->DrawTexture(iconRect, style.panelOpened.texture, style.panelOpened.fillColor);
				xOffset += iconRect.m_dimension.x + 5;
			}
		}

		UIRect headerRect;
		headerRect.m_pos = panelRect.m_pos + UIVector2{ xOffset, 0 };
		headerRect.m_dimension.x = panelRect.m_dimension.x - padding * 2;
		headerRect.m_dimension.y = headerHeight;

		bool mouseInside = iconRect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });
		bool bPressed = false;
		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside)
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; bPressed = true; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = -1; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}

		if (bPressed)
		{
			bClosed = !bClosed;
		}

		DrawTextInRect(-99, headerRect, text, UIVector4{ 1.0f }, ZE::TEXT_LEFT, TEXT_V_CENTER, style.headerFontStyle.fontScale, font);

		contentPos.x = panelRect.m_pos.x + padding;
		contentPos.y = panelRect.m_pos.y + headerRect.m_dimension.y + padding;
	}

	void UI::DoDragablePanel(Int32 _id, UIRect& panelRect, const UIChar* text, Float32 padding, UIVector2& contentPos, const UIPanelStyle& style)
	{
		UIFont* font = style.headerFontStyle.font ? style.headerFontStyle.font : DefaultFont;
		UIRect headerRect;
		headerRect.m_pos = panelRect.m_pos + UIVector2{ padding, 0.0f };
		headerRect.m_dimension.x = panelRect.m_dimension.x - padding * 2;
		headerRect.m_dimension.y = style.headerHeight;

		bool bJustActive = false;
		bool mouseInside = headerRect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });
		if ((MainUIState.activeItem.id == _id || mouseInside) && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			bJustActive = MainUIState.activeItem.id != _id;
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside)
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = -1; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}

		if (MainUIState.activeItem.id == _id && !bJustActive)
		{
			UIVector2 delta{ MainUIState.mouseDeltaX, MainUIState.mouseDeltaY };
			headerRect.m_pos = headerRect.m_pos + delta;
			panelRect.m_pos = panelRect.m_pos + delta;
		}
		
		if (style.panel.texture)
		{
			MainUIState.drawer->DrawTexture(panelRect, style.panel.texture, style.panel.fillColor, style.panel.textureScale, style.panel.textureOffset);
		}
		else
		{
			MainUIState.drawer->DrawRect(panelRect, style.panel.fillColor);
		}

		//headerRect.m_dimension.y -= padding * 2;
		DrawTextInRect(-99, headerRect, text, UIVector4(1.0f), TEXT_LEFT, TEXT_V_CENTER, style.headerFontStyle.fontScale, font);

		contentPos.x = panelRect.m_pos.x + padding;
		contentPos.y = panelRect.m_pos.y + style.headerHeight + padding;
	}

	ZE::Float32 UI::DoSlider(Int32 _id, const UIRect& rect, Float32 percent, const UISliderStyle& sliderStyle)
	{
		bool mouseInside = rect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });

		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside)
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = -1; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = -1; }
		}

		if (MainUIState.activeItem.id == _id)
		{
			percent = (MainUIState.mouseX - rect.m_pos.x) * 1.0f / rect.m_dimension.x;
		}

		UIRect sliderRect;
		sliderRect.m_dimension.x = rect.m_dimension.x;
		sliderRect.m_dimension.y = rect.m_dimension.y;
		sliderRect.m_pos.x = rect.m_pos.x;
		sliderRect.m_pos.y = rect.m_pos.y + (rect.m_dimension.y - sliderRect.m_dimension.y) / 2;

		MainUIState.drawer->DrawRect(sliderRect, sliderStyle.slider.fillColor);

		UIRect buttonRect;
		buttonRect.m_dimension.x = 10;
		buttonRect.m_dimension.y = rect.m_dimension.y;
		buttonRect.m_pos.x = rect.m_pos.x + ((rect.m_dimension.x - buttonRect.m_dimension.x) * percent);
		buttonRect.m_pos.y = rect.m_pos.y;

		if (MainUIState.activeItem.id == _id)
		{
			MainUIState.drawer->DrawRect(buttonRect, sliderStyle.sliderButton.down.fillColor);
		}
		else if (MainUIState.hotItem.id == _id)
		{
			MainUIState.drawer->DrawRect(buttonRect, sliderStyle.sliderButton.hover.fillColor);
		}
		else
		{
			MainUIState.drawer->DrawRect(buttonRect, sliderStyle.sliderButton.up.fillColor);
		}

		return percent;

	}

	void UI::DrawTextInPos(Int32 _id, UIVector2& pos, const UIChar* text, const UIVector4& fillColor, UIFont* font, Float32 scale)
	{
		MainUIState.drawer->DrawText(pos, fillColor, font, text, scale);
	}

	void UI::DrawTextInRect(Int32 _id, UIRect& rect, const UIChar* text, UIVector4& fillColor, ETextAlign textAlign /*= TEXT_LEFT*/, ETextVerticalAlign vAlign /*= TEXT_V_CENTER*/, Float32 scale /*= 1.0f*/, UIFont* font)
	{
		UIVector2 textDimension;
		textDimension.x = font->calculateTextLength(text, scale);
		textDimension.y = font->calculateTextHeight(scale);

		UIVector2 actualPos = rect.m_pos;
		switch (textAlign)
		{
		case ZE::TEXT_CENTER:
			actualPos.x += (rect.m_dimension.x - textDimension.x) / 2;
			break;
		case ZE::TEXT_RIGHT:
			actualPos.x += (rect.m_dimension.x - textDimension.x);
			break;
		}

		switch (vAlign)
		{
		case ZE::TEXT_V_BOTTOM:
			actualPos.y += (rect.m_dimension.y - textDimension.y);
			break;
		case ZE::TEXT_V_CENTER:
			actualPos.y += (rect.m_dimension.y - textDimension.y) / 2;
			break;
		}

		MainUIState.drawer->DrawText(actualPos, fillColor, font, text, scale);
	}

	void UI::DrawMultiLineText(Int32 _id, const UIRect& rect, const UIChar* text, const UIVector4& fillColor, ETextAlign textAlign /*= TEXT_V_CENTER*/, ETextVerticalAlign vAlign /*= TEXT_V_TOP*/, Float32 scale /*= 1.0f*/, UIFont* font)
	{
		UIVector2 textDimension;
		textDimension.x = rect.m_dimension.x;
		textDimension.y = font->calculateTextHeight(scale);

		UIVector2 actualPos = rect.m_pos;

		switch (vAlign)
		{
		case ZE::TEXT_V_BOTTOM:
			actualPos.y += (rect.m_dimension.y - textDimension.y);
			break;
		case ZE::TEXT_V_CENTER:
			actualPos.y += (rect.m_dimension.y - textDimension.y) / 2;
			break;
		}

		MainUIState.drawer->DrawText(actualPos, fillColor, font, text, scale, true, rect.m_dimension.x, textAlign);
	}

	void UI::DrawTexture(Int32 _id, const UIVector2& pos, UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale, const UIVector4& scaleOffset)
	{
		UIRect rect;
		rect.m_pos = pos;
		rect.m_dimension = { (Float32)texture->getWidth(), (Float32)texture->getHeight() };
		MainUIState.drawer->DrawTexture(rect, texture, colorMultiplier, textureScale, scaleOffset);
	}

	void UI::ProcessDrawList()
	{
		MainUIState.renderer->ProcessCurrentDrawList();
	}

	ZE::UITexture* UI::LoadTexture(const char* filePath)
	{
		return UITextureManager::LoadTexture(filePath, MainUIState.renderer);
	}

	ZE::Float32 UI::Lerp(Float32 start, Float32 next, Float32 alpha)
	{
		return start + alpha * (next-start);
	}

	void UI::DrawTexture(Int32 _id, const UIRect& rect, UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale, const UIVector4& scaleOffset)
	{
		MainUIState.drawer->DrawTexture(rect, texture, colorMultiplier, textureScale, scaleOffset);
	}

	void UIDrawer::DrawTexture(const UIRect& rect, UITexture* texture, const UIVector4& fillColor, ETextureScale textureScale, const UIVector4& scaleOffset)
	{
#if defined(ZUI_GROUP_PER_TEXTURE) && defined(ZUI_USE_RECT_INSTANCING)
		UIDrawItem* drawItem = m_currentDrawList->getTextureInstanceDrawItem(texture->getTextureHandle());
#elif defined(ZUI_GROUP_PER_TEXTURE)
		UIDrawItem* drawItem = m_currentDrawList->getTextureDrawItem(texture->getTextureHandle());
#else
		UIDrawItem* drawItem = m_currentDrawList->getNextDrawItem();
		drawItem->m_textureHandle = texture->getTextureHandle();
	#if defined(ZUI_USE_RECT_INSTANCING)
		drawItem->m_bUsingRectInstance = true;
	#endif
#endif

		UIVector2 positions[4] = { rect.m_pos, 
								{ rect.m_pos.x + rect.m_dimension.x, rect.m_pos.y }, 
								{ rect.m_pos.x, rect.m_pos.y + rect.m_dimension.y }, 
								rect.m_pos + rect.m_dimension };
		UIVector2 texCoords[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };
		
		if (textureScale == ZE::SCALE_IMAGE)
		{
#if defined(ZUI_USE_RECT_INSTANCING)
			drawItem->m_instances.push_back(UIInstance{ rect.m_pos, m_currentDepth, rect.m_dimension, 0.0f, fillColor, UIVector4{ 0.0f, 0.0f, 1.0f, 1.0f } });
#else
			drawItem->m_vertices.push_back(UIVertex{ positions[0], m_currentDepth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[1], m_currentDepth, texCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[3], m_currentDepth, texCoords[3], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[0], m_currentDepth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[3], m_currentDepth, texCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[2], m_currentDepth, texCoords[2], fillColor });
#endif
		}
		else if (textureScale == SCALE_9SCALE || textureScale == SCALE_BORDER)
		{
			UIVector2 outerPositions[8] = { positions[0] + UIVector2{ Float32(texture->getWidth() * scaleOffset.z), 0 },
											positions[0] + UIVector2{ 0, Float32(texture->getHeight() * scaleOffset.x) },
											positions[1] + UIVector2{ Float32(texture->getWidth() * -scaleOffset.w), 0 },
											positions[1] + UIVector2{ 0, Float32(texture->getHeight() * scaleOffset.x) },
											positions[2] + UIVector2{ Float32(texture->getWidth() * scaleOffset.z), 0 },
											positions[2] + UIVector2{ 0, Float32(texture->getHeight() * -scaleOffset.y) },
											positions[3] + UIVector2{ Float32(texture->getWidth() * -scaleOffset.w), 0 },
											positions[3] + UIVector2{ 0, Float32(texture->getHeight() * -scaleOffset.y) }
			};

			UIVector2 outerTexCoords[8] = { { scaleOffset.z, 0.0f },
											{ 0.0f, scaleOffset.x },
											{ 1.0f - scaleOffset.w, 0.0f },
											{ 1.0f, scaleOffset.x },
											{ scaleOffset.z, 1.0f },
											{ 0.0f, 1.0f - scaleOffset.y },
											{ 1.0f - scaleOffset.w, 1.0f },
											{ 1.0f, 1.0f - scaleOffset.y } };

			UIVector2 innerPositions[4] = { positions[0] + UIVector2{ Float32(texture->getWidth() * scaleOffset.z), Float32(texture->getHeight() * scaleOffset.x) },
											positions[1] + UIVector2{ Float32(texture->getWidth() * -scaleOffset.w), Float32(texture->getHeight() * scaleOffset.x) },
											positions[2] + UIVector2{ Float32(texture->getWidth() * scaleOffset.z), Float32(texture->getHeight() * -scaleOffset.y) },
											positions[3] + UIVector2{ Float32(texture->getWidth() * -scaleOffset.w), Float32(texture->getHeight() * -scaleOffset.y) },
										};

			UIVector2 innerTexCoords[4] = { {scaleOffset.z, scaleOffset.x},
											{ 1.0f - scaleOffset.w, scaleOffset.x },
											{ scaleOffset.z, 1.0f - scaleOffset.y },
											{1.0f - scaleOffset.w, 1.0f - scaleOffset.y}
										};

#if defined(ZUI_USE_RECT_INSTANCING)

			Float32 top = scaleOffset.x;
			Float32 bottom = scaleOffset.y;
			Float32 left = scaleOffset.z;
			Float32 right = scaleOffset.w;

			UIVector2 smallRectDim1{ left * texture->getWidth(), top * texture->getHeight() };
			UIVector2 smallRectDim2{ right* texture->getWidth(), top * texture->getHeight() };
			UIVector2 smallRectDim3{ left * texture->getWidth(), bottom * texture->getHeight() };
			UIVector2 smallRectDim4{ right * texture->getWidth(), bottom * texture->getHeight() };
			
			UIVector2 edgeRectDimTop{ rect.m_dimension.x - smallRectDim1.x - smallRectDim2.x, smallRectDim1.y };
			UIVector2 edgeRectDimBottom{ rect.m_dimension.x - smallRectDim1.x - smallRectDim2.x, smallRectDim3.y };
			UIVector2 edgeRectDimLeft{ smallRectDim1.x, rect.m_dimension.y - smallRectDim1.y - smallRectDim3.y };
			UIVector2 edgeRectDimRight{ smallRectDim1.x, rect.m_dimension.y - smallRectDim2.y - smallRectDim4.y };

			// Top Left Corner
			drawItem->m_instances.push_back(UIInstance{ rect.m_pos, m_currentDepth, smallRectDim1, 0.0f, fillColor, UIVector4{ 0.0f, 0.0f, left, top } });

			// Bottom Left Corner
			drawItem->m_instances.push_back(UIInstance{ outerPositions[5], m_currentDepth, smallRectDim3, 0.0f, fillColor, UIVector4{ outerTexCoords[5].x, outerTexCoords[5].y, left, bottom } });

			// Right Top Corner
			drawItem->m_instances.push_back(UIInstance{ outerPositions[2], m_currentDepth, smallRectDim2, 0.0f, fillColor, UIVector4{ outerTexCoords[2].x, outerTexCoords[2].y, right, top } });

			// Right Bottom Corner
			drawItem->m_instances.push_back(UIInstance{ innerPositions[3], m_currentDepth, smallRectDim4, 0.0f, fillColor, UIVector4{ innerTexCoords[3].x, innerTexCoords[3].y, right, bottom } });
			
			// Top Edge
			drawItem->m_instances.push_back(UIInstance{ outerPositions[0], m_currentDepth, edgeRectDimTop, 0.0f, fillColor, UIVector4{ outerTexCoords[0].x, outerTexCoords[0].y, 1.0f - right - left, top } });
			
			// Bottom Edge
			drawItem->m_instances.push_back(UIInstance{ innerPositions[2], m_currentDepth, edgeRectDimBottom, 0.0f, fillColor, UIVector4{ innerTexCoords[2].x, innerTexCoords[2].y, 1.0f - right - left, bottom } });
			
			// Left Edge
			drawItem->m_instances.push_back(UIInstance{ outerPositions[1], m_currentDepth, edgeRectDimLeft, 0.0f, fillColor, UIVector4{ outerTexCoords[1].x, outerTexCoords[1].y, left, 1.0f - top - bottom } });

			// Right Edge
			drawItem->m_instances.push_back(UIInstance{ innerPositions[1], m_currentDepth, edgeRectDimRight, 0.0f, fillColor, UIVector4{ innerTexCoords[1].x, innerTexCoords[1].y, right, 1.0f - top - bottom } });

#else
			// Top-Left Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[0], m_currentDepth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[0], m_currentDepth, outerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], m_currentDepth, innerTexCoords[0], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[0], m_currentDepth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], m_currentDepth, innerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[1], m_currentDepth, outerTexCoords[1], fillColor });

			// Top-Right Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[1], m_currentDepth, texCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[3], m_currentDepth, outerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], m_currentDepth, innerTexCoords[1], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[1], m_currentDepth, texCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], m_currentDepth, innerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[2], m_currentDepth, outerTexCoords[2], fillColor });

			// Bottom-Left Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[2], m_currentDepth, texCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[5], m_currentDepth, outerTexCoords[5], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], m_currentDepth, innerTexCoords[2], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[2], m_currentDepth, texCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], m_currentDepth, innerTexCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[4], m_currentDepth, outerTexCoords[4], fillColor });

			// Bottom-Right Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[3], m_currentDepth, texCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[7], m_currentDepth, outerTexCoords[7], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], m_currentDepth, innerTexCoords[3], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[3], m_currentDepth, texCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], m_currentDepth, innerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[6], m_currentDepth, outerTexCoords[6], fillColor });

			// Top
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[0], m_currentDepth, outerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[2], m_currentDepth, outerTexCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], m_currentDepth, innerTexCoords[1], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[0], m_currentDepth, outerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], m_currentDepth, innerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], m_currentDepth, innerTexCoords[0], fillColor });

			// Bottom
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[4], m_currentDepth, outerTexCoords[4], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], m_currentDepth, innerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], m_currentDepth, innerTexCoords[2], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[6], m_currentDepth, outerTexCoords[6], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[4], m_currentDepth, outerTexCoords[4], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], m_currentDepth, innerTexCoords[3], fillColor });

			// Left
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[1], m_currentDepth, outerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], m_currentDepth, innerTexCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[5], m_currentDepth, outerTexCoords[5], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[1], m_currentDepth, outerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], m_currentDepth, innerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], m_currentDepth, innerTexCoords[2], fillColor });

			// Right
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[3], m_currentDepth, outerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[7], m_currentDepth, outerTexCoords[7], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], m_currentDepth, innerTexCoords[1], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[7], m_currentDepth, outerTexCoords[7], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], m_currentDepth, innerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], m_currentDepth, innerTexCoords[1], fillColor });
#endif
			if (textureScale == SCALE_9SCALE)
			{
#if defined(ZUI_USE_RECT_INSTANCING)
				drawItem->m_instances.push_back(UIInstance{ innerPositions[0], m_currentDepth, { rect.m_dimension.x - smallRectDim1.x - smallRectDim2.x, rect.m_dimension.y - smallRectDim1.y - smallRectDim3.y }, 0.0f, fillColor, UIVector4{ innerTexCoords[0].x, innerTexCoords[0].y, 1.0f - left - right, 1.0f - top - bottom } });
#else
				// Center
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], m_currentDepth, innerTexCoords[0], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], m_currentDepth, innerTexCoords[1], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], m_currentDepth, innerTexCoords[3], fillColor });

				drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], m_currentDepth, innerTexCoords[0], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], m_currentDepth, innerTexCoords[3], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], m_currentDepth, innerTexCoords[2], fillColor });
#endif
			}
		}
		m_currentDepth += m_step;
	}

	void UIDrawer::DrawRect(const UIRect& rect, const UIVector4& fillColor)
	{
#if defined(ZUI_GROUP_PER_TEXTURE) && defined(ZUI_USE_RECT_INSTANCING)
		UIDrawItem* drawItem = m_currentDrawList->getTextureInstanceDrawItem(0); // Zero for non texture
#elif defined(ZUI_GROUP_PER_TEXTURE)
		UIDrawItem* drawItem = m_currentDrawList->getTextureDrawItem(0); // Zero for non texture
#else
		UIDrawItem* drawItem = m_currentDrawList->getNextDrawItem();
#endif

#if defined(ZUI_USE_RECT_INSTANCING)
		drawItem->m_instances.push_back(UIInstance{ rect.m_pos, m_currentDepth, rect.m_dimension, rect.m_roundness, fillColor });
		drawItem->m_bUsingRectInstance = true;
#else
		UIVector2 positions[4] = { rect.m_pos,
		{ rect.m_pos.x + rect.m_dimension.x, rect.m_pos.y },
		{ rect.m_pos.x, rect.m_pos.y + rect.m_dimension.y },
			rect.m_pos + rect.m_dimension };
		UIVector2 texCoords[4] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } };

		drawItem->m_vertices.push_back(UIVertex{ positions[0], m_currentDepth, texCoords[0], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[1], m_currentDepth, texCoords[1], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[3], m_currentDepth, texCoords[3], fillColor });

		drawItem->m_vertices.push_back(UIVertex{ positions[0], m_currentDepth, texCoords[0], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[3], m_currentDepth, texCoords[3], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[2], m_currentDepth, texCoords[2], fillColor });
		
		drawItem->m_roundness = rect.m_roundness;
		drawItem->m_shapeDimension = rect.m_dimension;
#endif
		m_currentDepth += m_step;
	}

	void UIDrawer::DrawShape(UIArray<UIVector2>& points, const UIVector4& fillColor)
	{

	}

	void UIDrawer::DrawText(UIVector2& pos, const UIVector4& fillColor, UIFont* font, const UIChar* text, Float32 scale /*= 1.0f*/, bool bWordWrap /*= false*/, Float32 maxWidth /*= 0*/, ETextAlign wrapTextAlign /*= TEXT_LEFT*/)
	{
#if defined(ZUI_USE_FONT_INSTANCING) && !defined(ZUI_USE_SINGLE_TEXT_ONLY)
		UIDrawItem* drawItem = m_currentDrawList->getTextureInstanceDrawItem(font->getTextureHandle());
#elif defined(ZUI_USE_SINGLE_TEXT_ONLY) || !defined(ZUI_GROUP_PER_TEXTURE)
		UIDrawItem* drawItem = m_currentDrawList->getNextDrawItem();
		drawItem->m_textureHandle = font->getTextureHandle();
#else
		UIDrawItem* drawItem = m_currentDrawList->getTextureDrawItem(font->getTextureHandle());
#endif
		if (bWordWrap)
		{
#if defined(ZUI_USE_FONT_INSTANCING)
			font->generateWrapTextInstanceBuffer(text, drawItem->m_instances, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentDepth, fillColor, maxWidth, wrapTextAlign);
#else
			font->generateWrapTextVertexBuffer(text, drawItem->m_vertices, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentDepth, fillColor, maxWidth, wrapTextAlign);
#endif
		}
		else
		{
#if defined(ZUI_USE_FONT_INSTANCING)
			font->generateInstanceBuffer(text, drawItem->m_instances, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentDepth, fillColor);
#else
			font->generateVertexBuffer(text, drawItem->m_vertices, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentDepth, fillColor);
#endif
		}

#if defined(ZUI_USE_SINGLE_TEXT_ONLY)
		drawItem->m_bUsingRectInstance = true;
#endif
		drawItem->m_bFont = true;
#if defined(ZUI_GROUP_PER_TEXTURE)
		drawItem->m_pass = 1;
#endif

		m_currentDepth += m_step;
	}

	void UIDrawer::Reset()
	{
		m_currentDrawList->reset();
		m_currentDepth = m_step;
	}

	void UIDrawer::SwapBuffer()
	{
		m_currentDrawList = m_currentDrawList == &m_drawLists[0] ? &m_drawLists[1] : &m_drawLists[0];
	}


	ZE::Int32 UIDrawList::itemCount() const
	{
		return m_count;
	}

	ZE::UIDrawItem* UIDrawList::getDrawItem(int _index) const
	{
		return m_drawItems[_index];
	}

	ZE::UIDrawItem* UIDrawList::getNextDrawItem()
	{
		if (m_count >= m_drawItems.size())
		{
			m_drawItems.push_back(UINEW(UIDrawItem));
		}

		return m_drawItems[m_count++];
	}

	ZE::UIDrawItem* UIDrawList::getTextureInstanceDrawItem(UInt32 _index)
	{
		int idx;
		HashMapHasAndAssign(m_textureToInstanceDrawItem, _index, -1, idx);
		if (idx == -1)
		{
			UIDrawItem* draw = getNextDrawItem();
			draw->m_textureHandle = _index;
			draw->m_bUsingRectInstance = true;
			m_textureToInstanceDrawItem[_index] = m_count - 1;
			idx = m_count - 1;
		}

		return m_drawItems[idx];
	}

	void UIDrawList::reset()
	{
		for (size_t i = 0; i < m_drawItems.size(); i++)
		{
			m_drawItems[i]->reset();
		}
		m_count = 0;
		m_textureToDrawItemMap.clear();
		m_textureToInstanceDrawItem.clear();
	}

	void UIDrawList::freeDrawItems()
	{
		for (size_t i = 0; i < m_drawItems.size(); i++)
		{
			UIFREE(m_drawItems[i]);
		}
	}

	ZE::UIDrawItem* UIDrawList::getTextureDrawItem(UInt32 textureHandle)
	{
		int idx;
		HashMapHasAndAssign(m_textureToDrawItemMap, textureHandle, -1, idx);
		if (idx == -1)
		{
			UIDrawItem* draw = getNextDrawItem();
			draw->m_textureHandle = textureHandle;
			m_textureToDrawItemMap[textureHandle] = m_count-1;
			idx = m_count - 1;
		}

		return m_drawItems[idx];
	}

	UIArray<ZE::UIVertex>& UIDrawItem::getVertices()
	{
		return m_vertices;
	}

	UIArray<ZE::UIInstance>& UIDrawItem::getInstances()
	{
		return m_instances;
	}

	void UIDrawItem::reset()
	{
		m_vertices.clear();
		m_instances.clear();

		m_textureHandle = 0;
		m_roundness = 0.0f;
		m_shapeDimension.x = m_shapeDimension.y = 0;

		m_bFont = false;
		m_bUsingRectInstance = false;
		m_pass = 0;
	}

	bool UIRect::isContain(const UIVector2& pos) const
	{
		UIVector2 diff = pos - m_pos;

		return diff.x >= 0 && diff.x <= m_dimension.x && diff.y >= 0 && diff.y <= m_dimension.y;
	}

	ZE::UIVector2 operator+(const UIVector2& v1, const UIVector2& v2)
	{
		return UIVector2{ v1.x + v2.x, v1.y + v2.y };
	}

	ZE::UIVector2 operator-(const UIVector2& v1, const UIVector2& v2)
	{
		return UIVector2{ v1.x - v2.x, v1.y - v2.y };
	}

	void CopyTexture(void* descMemory, Int32 width, Int32 height, Int32 targetX, Int32 targetY, void* srcMemory, Int32 srcWidth, Int32 srcHeight, size_t itemSize)
	{
		void* pDest = (void*)(reinterpret_cast<uintptr_t>(descMemory) + (itemSize * width * targetY) + (targetX * itemSize));
		void* pSource = srcMemory;
		for (Int32 i = 0; i < srcHeight; i++)
		{
			UIMEM_CPY(pDest, pSource, itemSize * srcWidth);
			pDest = (void*)(reinterpret_cast<uintptr_t>(pDest) + (itemSize * width));
			pSource = (void*)(reinterpret_cast<uintptr_t>(pSource) + (itemSize * srcWidth));
		}
	}

	ZE::UIFont* UIFont::loadFontFile(const UIChar* fontFilePath, UIRenderer* renderer, Int32 fontSizePx /*= 15*/)
	{
		UIFont* font = UINEW(UIFont);

		FT_Library ftLib;
		if (FT_Init_FreeType(&ftLib))
		{
			std::cout << "Couldn't init FreeType Library" << std::endl;
			return font;
		}

		FT_Face ftFace;
		if (FT_New_Face(ftLib, fontFilePath, 0, &ftFace))
		{
			std::cout << "Couldn't load Font" << std::endl;
			FT_Done_FreeType(ftLib);
			return font;
		}

		FT_Set_Pixel_Sizes(ftFace, 0, fontSizePx);

		UInt32 charCount = ftFace->num_glyphs;

		font->m_charDesc.resize(charCount);
		font->m_fontSize = fontSizePx;

		// Calculate Texture Atlas Size
		Float32 padding = 5.0f;
		Int32 numberPerRow = (Int32)(sqrt((fontSizePx + padding) / (0.5f * fontSizePx + padding) * charCount));

		UIVector2 curPos{ padding, padding };
		Int32 texSize = 5;
		for (UInt32 c = 0; c < charCount; c++)
		{
			if (FT_Load_Glyph(ftFace, c, FT_LOAD_BITMAP_METRICS_ONLY))
			{
				continue;
			}

			font->m_charDesc[c].TexCoord = curPos;

			curPos.x = curPos.x + padding + ftFace->glyph->bitmap.width;
			
			if (texSize < curPos.x + padding)
			{
				texSize = (Int32)(curPos.x + padding);
			}

			if ((c + 1) % numberPerRow == 0)
			{
				curPos.x = padding;
				curPos.y = curPos.y + padding + fontSizePx;
			}

			if (texSize < curPos.y + padding)
			{
				texSize = (Int32)(curPos.y + padding);
			}
		}

		// Texture atlas size must be multiply by 4
		texSize = (texSize / 4 + 1) * 4;
		font->m_fontAtlasSize = texSize;

		// Init Memory for texture atlas
		char* pAtlas = UINEW(char[texSize * texSize]);
		UIMEM_ZERO(pAtlas, sizeof(char) * texSize * texSize);

		// Load Glyph bitmap and copy to the Atlas
		for (UInt32 c = 0; c < charCount; c++)
		{
			if (FT_Load_Glyph(ftFace, c, FT_LOAD_RENDER))
			{
				continue;
			}

			UIFontCharDesc& desc = font->m_charDesc[c];
			desc.Dimension = { (Float32)ftFace->glyph->bitmap.width, (Float32)ftFace->glyph->bitmap.rows };
			desc.Advance = ftFace->glyph->advance.x >> 6;
			desc.Bearing = { 1.0f * ftFace->glyph->bitmap_left, 1.0f * ftFace->glyph->bitmap_top };

			CopyTexture(pAtlas, texSize, texSize, static_cast<Int32>(desc.TexCoord.x), static_cast<Int32>(desc.TexCoord.y), ftFace->glyph->bitmap.buffer, ftFace->glyph->bitmap.width, ftFace->glyph->bitmap.rows, sizeof(char));
		} 

		// Generate Map
		UInt32 glyphIndex = 0;
		UInt32 charCode = FT_Get_First_Char(ftFace, &glyphIndex);
		while (glyphIndex != 0)
		{
			font->m_charMap[charCode] = glyphIndex;
			charCode = FT_Get_Next_Char(ftFace, charCode, &glyphIndex);
		}

		font->m_textureHandle = renderer->createRendererTexture(pAtlas, texSize, texSize, 1);

		UIFREE(pAtlas);

		FT_Done_Face(ftFace);
		FT_Done_FreeType(ftLib);

		font->m_renderer = renderer;

		return font;
	}

	ZE::Float32 UIFont::calculateTextLength(const UIChar* text, Float32 scale)
	{
		int index = 0;
		char c = text[index++];
		Float32 result = 0;
		while (c != '\0')
		{
			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			result += scale * m_charDesc[charIndex].Advance;
			c = text[index++];
		}

		return result;
	}

	ZE::Float32 UIFont::calculateTextHeight(Float32 scale)
	{
		return m_fontSize * scale;
	}

	ZE::Float32 UIFont::calculateWordWrapTextHeight(const UIChar* text, Float32 scale, Int32 maxWidth)
	{
		int index = 0;
		char c = text[index++];

		Float32 height = 0;
		Float32 cX = 0;
		while (c != '\0')
		{
			if (c == '\n')
			{
				height += calculateTextHeight(scale);
				c = text[index++];
				cX = 0;
			}

			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			UIFontCharDesc& charDesc = m_charDesc[charIndex];

			cX += charDesc.Advance * scale;
			if (cX > maxWidth)
			{
				height += calculateTextHeight(scale);
				cX = 0;
				index--;
			}
			else
			{
				c = text[index++];
			}
		}

		return height;
	}

	void UIFont::generateVertexBuffer(const UIChar* text, UIArray<UIVertex>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color)
	{
		int index = 0;
		char c = text[index++];

		Float32 x = pos.x;
		Float32 y = -pos.y;

		while (c != '\0')
		{
			if (c == '\n')
			{
				c = text[index++];
				y -= m_fontSize * scale;
				x = pos.x;
				continue;
			}

			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			
			UIFontCharDesc& charDesc = m_charDesc[charIndex];

			Float32 xPos = x + charDesc.Bearing.x * scale;
			Float32 yPos = y + (charDesc.Bearing.y - charDesc.Dimension.y) * scale;

			Float32 w = charDesc.Dimension.x * scale;
			Float32 h = charDesc.Dimension.y * scale;

			result.push_back(UIVertex{ UIVector2{ xPos, yPos }, depth, UIVector2{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, (charDesc.TexCoord.y + charDesc.Dimension.y) * 1.0f / m_fontAtlasSize }, color });
			result.push_back(UIVertex{ UIVector2{ xPos + w, yPos }, depth, UIVector2{ (charDesc.TexCoord.x + charDesc.Dimension.x) * 1.0f / m_fontAtlasSize, (charDesc.TexCoord.y + charDesc.Dimension.y) * 1.0f / m_fontAtlasSize }, color });
			result.push_back(UIVertex{ UIVector2{ xPos, yPos + h }, depth, UIVector2{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize }, color });
			
			result.push_back(UIVertex{ UIVector2{ xPos, yPos + h }, depth, UIVector2{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize }, color });
			result.push_back(UIVertex{ UIVector2{ xPos + w, yPos }, depth, UIVector2{ (charDesc.TexCoord.x + charDesc.Dimension.x) * 1.0f / m_fontAtlasSize, (charDesc.TexCoord.y + charDesc.Dimension.y) * 1.0f / m_fontAtlasSize }, color });
			result.push_back(UIVertex{ UIVector2{ xPos + w, yPos + h }, depth, UIVector2{ (charDesc.TexCoord.x + charDesc.Dimension.x) * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize }, color });
			
			x += charDesc.Advance * scale;
			c = text[index++];
		}
	}

	void UIFont::generateWrapTextVertexBuffer(const UIChar* text, UIArray<UIVertex>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color, Float32 maxWidth, ETextAlign textAlign)
	{
		UIArray<UInt32> charIndices;
		Float32 width;
		Int32 count = 0;
		Float32 x;
		Float32 y = -pos.y;

		while (generateWrapTextBuffer(text + count, charIndices, scale, depth, maxWidth, width, count))
		{
			x = pos.x;
			switch (textAlign)
			{
			case TEXT_CENTER:
				x += (maxWidth - width) / 2;
				break;
			case TEXT_RIGHT:
				x += maxWidth - width;
				break;
			}

			for (size_t i = 0; i < charIndices.size(); i++)
			{
				UIFontCharDesc& charDesc = m_charDesc[charIndices[i]];

				Float32 xPos = x + charDesc.Bearing.x * scale;
				Float32 yPos = y + (charDesc.Bearing.y - charDesc.Dimension.y) * scale;

				Float32 w = charDesc.Dimension.x * scale;
				Float32 h = charDesc.Dimension.y * scale;

				result.push_back(UIVertex{ UIVector2{ xPos, yPos }, depth, UIVector2{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, (charDesc.TexCoord.y + charDesc.Dimension.y) * 1.0f / m_fontAtlasSize }, color });
				result.push_back(UIVertex{ UIVector2{ xPos + w, yPos }, depth, UIVector2{ (charDesc.TexCoord.x + charDesc.Dimension.x) * 1.0f / m_fontAtlasSize, (charDesc.TexCoord.y + charDesc.Dimension.y) * 1.0f / m_fontAtlasSize }, color });
				result.push_back(UIVertex{ UIVector2{ xPos, yPos + h }, depth, UIVector2{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize }, color });

				result.push_back(UIVertex{ UIVector2{ xPos, yPos + h }, depth, UIVector2{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize }, color });
				result.push_back(UIVertex{ UIVector2{ xPos + w, yPos }, depth, UIVector2{ (charDesc.TexCoord.x + charDesc.Dimension.x) * 1.0f / m_fontAtlasSize, (charDesc.TexCoord.y + charDesc.Dimension.y) * 1.0f / m_fontAtlasSize }, color });
				result.push_back(UIVertex{ UIVector2{ xPos + w, yPos + h }, depth, UIVector2{ (charDesc.TexCoord.x + charDesc.Dimension.x) * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize }, color });
				
				x += charDesc.Advance * scale;
			}

			y -= calculateTextHeight(scale);
			charIndices.clear();
		}
	}

	void UIFont::generateInstanceBuffer(const UIChar* text, UIArray<UIInstance>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color)
	{
		int index = 0;
		char c = text[index++];

		Float32 x = pos.x;
		Float32 y = -pos.y;

		while (c != '\0')
		{
			if (c == '\n')
			{
				c = text[index++];
				y -= m_fontSize * scale;
				x = pos.x;
				continue;
			}

			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			UIFontCharDesc& charDesc = m_charDesc[charIndex];

			Float32 xPos = x + charDesc.Bearing.x * scale;
			Float32 yPos = y + (charDesc.Bearing.y - charDesc.Dimension.y) * scale;

			Float32 w = charDesc.Dimension.x * scale;
			Float32 h = charDesc.Dimension.y * scale;

			result.push_back(UIInstance{ UIVector2{ xPos, yPos }, depth, UIVector2{ w, h }, 0.0f, color, UIVector4{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize, charDesc.Dimension.x * 1.0f / m_fontAtlasSize, charDesc.Dimension.y * 1.0f / m_fontAtlasSize } });
			
			x += charDesc.Advance * scale;
			c = text[index++];
		}
	}

	void UIFont::generateWrapTextInstanceBuffer(const UIChar* text, UIArray<UIInstance>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color, Float32 maxWidth, ETextAlign textAlign)
	{
		UIArray<UInt32> charIndices;
		Float32 width;
		Int32 count = 0;
		Float32 x;
		Float32 y = -pos.y;

		while (generateWrapTextBuffer(text + count, charIndices, scale, depth, maxWidth, width, count))
		{
			x = pos.x;
			switch (textAlign)
			{
			case TEXT_CENTER:
				x += (maxWidth - width) / 2;
				break;
			case TEXT_RIGHT:
				x += maxWidth - width;
				break;
			}

			for (size_t i = 0; i < charIndices.size(); i++)
			{
				UIFontCharDesc& charDesc = m_charDesc[charIndices[i]];

				Float32 xPos = x + charDesc.Bearing.x * scale;
				Float32 yPos = y + (charDesc.Bearing.y - charDesc.Dimension.y) * scale;

				Float32 w = charDesc.Dimension.x * scale;
				Float32 h = charDesc.Dimension.y * scale;

				result.push_back(UIInstance{ UIVector2{ xPos, yPos }, depth, UIVector2{ w, h }, 0.0f, color, UIVector4{ charDesc.TexCoord.x * 1.0f / m_fontAtlasSize, charDesc.TexCoord.y * 1.0f / m_fontAtlasSize, charDesc.Dimension.x * 1.0f / m_fontAtlasSize, charDesc.Dimension.y * 1.0f / m_fontAtlasSize } });

				x += charDesc.Advance * scale;
			}

			y -= calculateTextHeight(scale);
			charIndices.clear();
		}
	}

	void UIFont::release()
	{
		if (m_textureHandle > 0)
		{
			m_renderer->destroyTexture(m_textureHandle);
			m_textureHandle = 0;
		}
	}

	bool UIFont::generateWrapTextBuffer(const UIChar* text, UIArray<UInt32>& charIndices, Float32 scale, Float32 depth, Float32 maxWidth, Float32& width, Int32& count)
	{
		char c = text[0];

		if (c == '\0') { return false; }
		width = 0;
		while (c != '\0' && c!= '\n')
		{
			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			UIFontCharDesc& charDesc = m_charDesc[charIndex];

			if (width + charDesc.Advance * scale >= maxWidth)
			{
				return true;
			}
			
			charIndices.push_back(charIndex);

			text++;
			c = text[0];
			width += charDesc.Advance * scale;
			count++;
		}

		if ( c == '\n' ) { count++; }
		return true;

	}

	ZE::Int32 UIFont::calculateFirstLineWidth(const UIChar* text, Float32 scale, Int32 maxWidth)
	{
		int index = 0;
		char c = text[index++];

		Float32 cX = 0;
		while (c != '\0' && c!= '\n')
		{
			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			UIFontCharDesc& charDesc = m_charDesc[charIndex];

			if (cX + charDesc.Advance * scale >= maxWidth)
			{
				return static_cast<Int32>(cX);
			}
			
			c = text[index++];
			cX += charDesc.Advance * scale;
		}

		return static_cast<Int32>(cX);
	}

	void UITextureManager::Init()
	{
		if (!s_instance)
		{
			s_instance = UINEW(UITextureManager);
		}
	}

	ZE::UITexture* UITextureManager::LoadTexture(const UIChar* fontFilePath, UIRenderer* renderer)
	{
		UITexture* resTexture = UINEW(UITexture);

		int width;
		int height;
		int channel;
		stbi_uc* image = stbi_load(fontFilePath, &width, &height, &channel, 0);

		if (image)
		{
			resTexture->m_textureHandle = renderer->createRendererTexture(image, width, height, channel);
			resTexture->m_width = width;
			resTexture->m_height = height;
			resTexture->m_renderer = renderer;

			stbi_image_free(image);

			if (s_instance)
			{
				s_instance->m_textures.push_back(resTexture);
			}
		}
		else
		{
			return nullptr;
		}


		return resTexture;
	}

	void UITextureManager::Destroy()
	{
		if (s_instance)
		{
			for (int i = 0; i < static_cast<int>(s_instance->m_textures.size()); i++)
			{
				UIFREE(s_instance->m_textures[i]);
			}

			s_instance->m_textures.clear();

			UIFREE(s_instance);
		}
	}

	ZE::UITextureManager* UITextureManager::s_instance;

	void UITexture::release()
	{
		if (m_textureHandle > 0)
		{
			m_renderer->destroyTexture(m_textureHandle);
			m_textureHandle = 0;
		}
	}

}