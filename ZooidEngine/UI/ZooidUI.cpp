#include "ZooidUI.h"

#include "ft2build.h"
#include FT_FREETYPE_H

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "UIRenderer.h"

#include "External/Image/stb_image.h"

#define UIMEM_CPY(Dst, Src, _Size) memcpy(Dst, Src, _Size)
#define UIMEM_MOVE(Dst, Src, _Size) memmove(Dst, Src, _Size)
#define UIMEM_ZERO(Dst, _Size) memset(Dst, 0, _Size)
#define UIMAX(a, b) (a > b ? (a) : (b))
#define UIMIN(a, b) (a < b ? (a) : (b))

#define MERGE_PATH(p1, p2) p1 p2

namespace ZE
{
	// Utility functions
	// Precomputed CRC32 Table
	static UInt32 crc_32_tab[] = { /* CRC polynomial 0xedb88320 */
		0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,
		0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
		0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,
		0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
		0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,
		0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
		0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,
		0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
		0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,
		0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
		0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,
		0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
		0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,
		0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
		0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,
		0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
		0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,
		0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
		0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,
		0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
		0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,
		0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
		0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,
		0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
		0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,
		0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
		0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,
		0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
		0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,
		0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
		0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,
		0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
		0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,
		0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
		0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,
		0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
		0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,
		0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
		0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,
		0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
		0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,
		0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
		0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d
	};

	ZE::UInt32 ComputeHashString(const UIChar* pData, UInt32 dataSize, UInt32 seed /*= 0*/)
	{
		UInt32 crc = ~seed;
		const UIChar* data = pData;
		for (; dataSize > 0 && (*data); --dataSize, ++data)
		{
			crc = crc_32_tab[(crc ^ ((uint8_t) *data)) & 0xff] ^ ((crc)>>8);
		}

		return ~crc;
	}

	ZE::UInt32 ComputeHashData(void* pData, UInt32 dataSize, UInt32 seed /*= 0*/)
	{
		UInt32 crc = ~seed;
		UIChar* data = (UIChar*)pData;

		for (; dataSize; --dataSize, ++data)
		{
			crc = crc_32_tab[(crc ^ ((uint8_t)*data)) & 0xff] ^ ((crc) >> 8);
		}

		return ~crc;
	}

	ZE::UInt32 ComputeHashPointer(const void* pData, UInt32 seed /*= 0*/)
	{
		return ComputeHashData(&pData, sizeof(void*), seed);
	}

	// Cache Id : Basically just prev active id
	UInt32 cacheId;

	UIState UI::MainUIState;

	UIButtonStyle UI::DefaultButtonStyle;
	UIButtonStyle UI::DefaultCheckBoxStyle;
	UIButtonStyle UI::DefaultRadioBtnStyle;
	UISliderStyle UI::DefaultSliderStyle;
	UIPanelStyle UI::DefaultPanelStyle;
	UIDropdownStyle UI::DefaultDropdownStyle;
	UITextInputStyle UI::DefaultTextInputStyle;
	UIFontStyle UI::DefaultFontStyle;
	
	UIStack<UInt32> UI::StackIDs;

	UIFont* UI::DefaultFont = nullptr;

	ZE::UIState* UI::GetUIState()
	{
		return &MainUIState;
	}

	ZE::UInt32 UI::GetUIIDFromString(const UIChar* textData, UInt32 length)
	{
		UInt32 parent = StackIDs.back();
		return ComputeHashString(textData, length, parent);
	}

	ZE::UInt32 UI::GetUIIDFromPointer(const void* pData)
	{
		UInt32 parent = StackIDs.back();
		return ComputeHashPointer(pData, parent);
	}

	// Helper function: Get Length of the UIChar buffer
	Int32 TextLength(const UIChar* text)
	{
		Int32 len = 0;
		while (text[len] != 0)
		{
			len++;
		}
		return len;
	}

	void UI::Init(Int32 width, Int32 height)
	{
		MainUIState.drawer = UINEW(UIDrawer);
		MainUIState.renderer = Platform::CreateRenderer();
		MainUIState.timeFromStart = 0.0f;

		MainUIState.renderer->Init(width, height);

		MainUIState.textTempLength = TEXT_TEMP_LENGTH;
		MainUIState.textTempBuffer = UINEW(UIChar[TEXT_TEMP_LENGTH]); // #TODO need to create UIAlloc

		MainUIState.textInputFilterChar = nullptr;

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
		ZE::UITexture* dropdownBtnUp = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/DropdownBtn_Up.png"), MainUIState.renderer);
		ZE::UITexture* dropdownBtnDown = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/DropdownBtn_Down.png"), MainUIState.renderer);
		ZE::UITexture* dropdownBtnHover = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/DropdownBtn_Hover.png"), MainUIState.renderer);
		ZE::UITexture* textInputBg = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/InputFieldBg.png"), MainUIState.renderer);
		ZE::UITexture* textInputBgActive = ZE::UITextureManager::LoadTexture(MERGE_PATH(RESOURCE_FOLDER, "Textures/InputFieldBgActive.png"), MainUIState.renderer);
		
		// Default Font
		DefaultFont = ZE::UIFont::loadFontFile(MERGE_PATH(RESOURCE_FOLDER, DEFAULT_FONT_PATH), MainUIState.renderer, 16);

		// Default Font Style
		DefaultFontStyle.font = DefaultFont;
		DefaultFontStyle.fontScale = 1.0f;

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
		DefaultButtonStyle.fontStyle = DefaultFontStyle;

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

		// Default Drowpdown Style
		DefaultDropdownStyle.dropdownButtonStyle.up.texture = dropdownBtnUp;
		DefaultDropdownStyle.dropdownButtonStyle.up.fillColor = UIVector4(1.0f);
		DefaultDropdownStyle.dropdownButtonStyle.up.textureOffset = UIVector4(0.12f, 0.12f, 0.075f, 0.4f);
		DefaultDropdownStyle.dropdownButtonStyle.up.textureScale = SCALE_9SCALE;
		DefaultDropdownStyle.dropdownButtonStyle.down.texture = dropdownBtnDown;
		DefaultDropdownStyle.dropdownButtonStyle.down.fillColor = UIVector4(1.0f);
		DefaultDropdownStyle.dropdownButtonStyle.down.textureOffset = UIVector4(0.12f, 0.12f, 0.075f, 0.4f);
		DefaultDropdownStyle.dropdownButtonStyle.down.textureScale = SCALE_9SCALE;
		DefaultDropdownStyle.dropdownButtonStyle.hover.texture = dropdownBtnHover;
		DefaultDropdownStyle.dropdownButtonStyle.hover.fillColor = UIVector4(1.0f);
		DefaultDropdownStyle.dropdownButtonStyle.hover.textureOffset = UIVector4(0.12f, 0.12f, 0.075f, 0.4f);
		DefaultDropdownStyle.dropdownButtonStyle.hover.textureScale = SCALE_9SCALE;
		DefaultDropdownStyle.dropdownButtonStyle.fontStyle.font = DefaultFont;
		DefaultDropdownStyle.dropdownButtonStyle.fontStyle.fontScale = 1.0f;
		DefaultDropdownStyle.selectorStyle.fillColor = UIVector4(0.0f, 0.5f, 1.0f, 1.0f);
		DefaultDropdownStyle.selectorHoverStyle.fillColor = UIVector4(0.0f, 0.0f, 1.0f, 0.5f);
		DefaultDropdownStyle.selectorFontStyle.font = DefaultFont;
		DefaultDropdownStyle.selectorFontStyle.fontScale = 1.0f;

		// Default Text Input Style
		DefaultTextInputStyle.defaultStyle.fillColor = UIVector4(1.0f);
		DefaultTextInputStyle.defaultStyle.texture = textInputBg;
		DefaultTextInputStyle.defaultStyle.textureScale = SCALE_9SCALE;
		DefaultTextInputStyle.defaultStyle.textureOffset = UIVector4(0.20f);
		DefaultTextInputStyle.activeStyle.fillColor = UIVector4(1.0f);
		DefaultTextInputStyle.activeStyle.texture = textInputBgActive;
		DefaultTextInputStyle.activeStyle.textureScale = SCALE_9SCALE;
		DefaultTextInputStyle.activeStyle.textureOffset = UIVector4(0.20f);
		DefaultTextInputStyle.fontStyle.font = DefaultFont;
		DefaultTextInputStyle.fontStyle.fontScale = 1.0f;

		// Push 0 as default stack id
		StackIDs.push_back(0);
	}

	void UI::ResizeWindow(Int32 width, Int32 height)
	{
		if (MainUIState.renderer)
		{
			MainUIState.renderer->resizeWindow(width, height);
		}
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

		if (MainUIState.textTempBuffer)
		{
			UIFREE(MainUIState.textTempBuffer);
		}
	}

	void UI::BeginFrame()
	{
		MainUIState.drawer->Reset();
		MainUIState.timeFromStart += MainUIState.mainTimer.ResetAndGetDeltaMS();
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

	void UI::RecordKeyboardButton(UIChar keyChar, int keyState)
	{
		// Handle Keys
		// Backspace key
		if (keyChar == ZOOID_KEY_BACKSPACE && (keyState == 0 || keyState == 2))
		{
			if (MainUIState.textInputCurrentPos > 0)
			{
				Int32 len = TextLength(MainUIState.textInputBuffer);
				UIMEM_MOVE(MainUIState.textInputBuffer + MainUIState.textInputCurrentPos - 1,
					MainUIState.textInputBuffer + MainUIState.textInputCurrentPos,
					len - MainUIState.textInputCurrentPos + 1);

				MainUIState.textInputCurrentPos--;

				if (MainUIState.textInputCurrentPos > MainUIState.textInputMaxScroll)
				{
					MainUIState.textInputScrollPos = MainUIState.textInputCurrentPos - MainUIState.textInputMaxScroll;
				}
				else
				{
					MainUIState.textInputScrollPos = 0;
				}
			}
			return;
		}
		else if (keyChar == ZOOID_KEY_ENTER)
		{
			return;
		}
		else if (keyChar == ZOOID_KEY_DELETE && (keyState == 0 || keyState == 2))
		{
			Int32 len = TextLength(MainUIState.textInputBuffer);
			if (MainUIState.textInputCurrentPos < len)
			{
				Int32 len = TextLength(MainUIState.textInputBuffer);
				UIMEM_MOVE(MainUIState.textInputBuffer + MainUIState.textInputCurrentPos,
					MainUIState.textInputBuffer + MainUIState.textInputCurrentPos + 1,
					len - MainUIState.textInputCurrentPos + 1);
			}
			return;
		}
		else if (keyChar == ZOOID_KEY_ARROW_LEFT && (keyState == 0 || keyState == 2))
		{
			if (MainUIState.textInputCurrentPos > 0)
			{
				MainUIState.textInputCurrentPos--;

				if (MainUIState.textInputCurrentPos < MainUIState.textInputScrollPos)
				{
					MainUIState.textInputScrollPos = MainUIState.textInputCurrentPos;
				}
			}
		}
		else if (keyChar == ZOOID_KEY_ARROW_RIGHT && (keyState == 0 || keyState == 2))
		{
			Int32 len = TextLength(MainUIState.textInputBuffer);
			if (MainUIState.textInputCurrentPos < len)
			{
				MainUIState.textInputCurrentPos++;

				if (MainUIState.textInputCurrentPos > MainUIState.textInputScrollPos + MainUIState.textInputMaxScroll)
				{
					MainUIState.textInputScrollPos = MainUIState.textInputCurrentPos - MainUIState.textInputMaxScroll;
				}
			}
		}
		else if (keyChar == ZOOID_KEY_HOME)
		{
			MainUIState.textInputCurrentPos = 0;
		}
		else if (keyChar == ZOOID_KEY_END)
		{
			Int32 len = TextLength(MainUIState.textInputBuffer);
			MainUIState.textInputCurrentPos = len;
		}
	}

	void UI::RecordTextInput(UIChar keyChar)
	{
		// Add to buffer if focus is textInput
		if (MainUIState.lastTextInput.id != 0 && MainUIState.lastTextInput.id == MainUIState.activeItem.id)
		{
			if (MainUIState.textInputFilterChar && !MainUIState.textInputFilterChar(keyChar)) { return; }
			Int32 len = TextLength(MainUIState.textInputBuffer);
			if (len < MainUIState.textInputLength - 1)
			{
				UIMEM_MOVE(MainUIState.textInputBuffer + MainUIState.textInputCurrentPos + 1,
					MainUIState.textInputBuffer + MainUIState.textInputCurrentPos,
					len - MainUIState.textInputCurrentPos + 1);
				MainUIState.textInputBuffer[MainUIState.textInputCurrentPos++] = keyChar;
				
				if (MainUIState.textInputCurrentPos > MainUIState.textInputMaxScroll)
				{
					MainUIState.textInputScrollPos = MainUIState.textInputCurrentPos - MainUIState.textInputMaxScroll;
				}
			}
		}
		else
		{
			MainUIState.lastTextInput.id = 0;
		}
	}

	bool UI::DoButton(const UIChar* label, const UIButtonStyle& buttonStyle /*= DefaultButtonStyle*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.x = buttonStyle.fontStyle.font->calculateTextLength(label, buttonStyle.fontStyle.fontScale) + 60.0f;
		rect.m_dimension.y = buttonStyle.fontStyle.font->calculateTextHeight(buttonStyle.fontStyle.fontScale) + 20.0f;

		bool result = DoButtonEx(label, rect, buttonStyle);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;

		return result;
	}

	bool UI::DoButtonEx(const UIChar* label, UIRect& rect, const UIButtonStyle& buttonStyle)
	{
		bool mouseInside = rect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });
		bool bPressed = false;
		
		UInt32 _id = GetUIIDFromString(label);

		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if ( mouseInside )
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; bPressed = true; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
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

		DrawTextInRect(rect, label, UIVector4(1.0f), TEXT_CENTER, TEXT_V_CENTER, buttonStyle.fontStyle.fontScale, buttonStyle.fontStyle.font);

		return bPressed;
	}

	bool UI::DoCheckBox(const UIChar* text, bool bChecked, const UIButtonStyle& checkBoxStyle /*= DefaultCheckBoxStyle*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;

		bool result = DoCheckBoxEx(MainUIState.drawPosDimension.m_pos, text, bChecked, rect, checkBoxStyle);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;

		return result;
	}

	bool UI::DoCheckBoxEx(const UIVector2& pos, const UIChar* text, bool bChecked, UIRect& rect, const UIButtonStyle& checkBoxStyle)
	{
		UIFont* font = checkBoxStyle.fontStyle.font ? checkBoxStyle.fontStyle.font : DefaultFont;
		rect.m_pos = pos;

		UInt32 _id = GetUIIDFromString(text);
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
			if (MainUIState.activeItem.id == _id) 
			{ 
				MainUIState.activeItem.id = 0; 
				bPressed = true; 
			}
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
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

	void UI::DoRadioButton(Int32 _buttonId, const UIVector2& pos, const UIChar* text, Int32* _selectedId, const UIButtonStyle& radioButtonStyle)
	{
		bool bChecked = _buttonId == *_selectedId;
		UInt32 _id = GetUIIDFromString(text);

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
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; bPressed = true; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
		}

		if (bPressed)
		{
			*_selectedId = _buttonId;
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
	}

	void UI::DoRadioButtons(const UIChar** textArray, UInt32 textArrayLen, Int32* _selectionId, const UIButtonStyle& radioButtonStyle /*= DefaultRadioBtnStyle*/)
	{
		DoRadioButtonsEx(MainUIState.drawPosDimension.m_pos, textArray, textArrayLen, _selectionId, radioButtonStyle);

		UIFont* font = radioButtonStyle.fontStyle.font ? radioButtonStyle.fontStyle.font : DefaultFont;
		Float32 height = font->calculateTextHeight(radioButtonStyle.fontStyle.fontScale) + 5.0f;
		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * UIVector2(100.0f, height * textArrayLen) + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoRadioButtonsEx(const UIVector2& startPos, const UIChar** textArray, UInt32 textArrayLen, Int32* _selectionId, const UIButtonStyle& radioButtonStyle /*= DefaultRadioBtnStyle*/)
	{
		UInt32 id = GetUIIDFromPointer(_selectionId);

		StackIDs.push_back(id);

		UIFont* font = radioButtonStyle.fontStyle.font ? radioButtonStyle.fontStyle.font : DefaultFont;

		Float32 height = font->calculateTextHeight(radioButtonStyle.fontStyle.fontScale) + 5.0f;
		UIVector2 pos = startPos;
		for (UInt32 i = 0; i < textArrayLen; i++)
		{
			DoRadioButton(i, pos, textArray[i], _selectionId, radioButtonStyle);
			pos.y += height;
		}
		StackIDs.pop_back();
	}

	void UI::DoPanel(const UIRect& panelRect, const UIChar* text, Float32 padding, UIVector2& contentPos, bool& bClosed, const UIPanelStyle& style)
	{
		UIFont* font = style.headerFontStyle.font ? style.headerFontStyle.font : DefaultFont;
		Float32 xOffset = padding;
		Float32 headerHeight = style.headerHeight;

		UInt32 _id = GetUIIDFromString(text);

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
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; bPressed = true; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
		}

		if (bPressed)
		{
			bClosed = !bClosed;
		}

		DrawTextInRect(headerRect, text, UIVector4{ 1.0f }, ZE::TEXT_LEFT, TEXT_V_CENTER, style.headerFontStyle.fontScale, font);

		contentPos.x = panelRect.m_pos.x + padding;
		contentPos.y = panelRect.m_pos.y + headerRect.m_dimension.y + padding;
	}

	void UI::DoDragablePanel(UIRect& panelRect, const UIChar* text, Float32 padding, UIVector2& contentPos, const UIPanelStyle& style)
	{
		UIFont* font = style.headerFontStyle.font ? style.headerFontStyle.font : DefaultFont;
		UIRect headerRect;
		headerRect.m_pos = panelRect.m_pos + UIVector2{ padding, 0.0f };
		headerRect.m_dimension.x = panelRect.m_dimension.x - padding * 2;
		headerRect.m_dimension.y = style.headerHeight;

		UInt32 _id = GetUIIDFromString(text);

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
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
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
		DrawTextInRect(headerRect, text, UIVector4(1.0f), TEXT_LEFT, TEXT_V_CENTER, style.headerFontStyle.fontScale, font);

		contentPos.x = panelRect.m_pos.x + padding;
		contentPos.y = panelRect.m_pos.y + style.headerHeight + padding;
	}

	void UI::DoSlider(Float32* percent, const UISliderStyle& sliderStyle /*= DefaultSliderStyle*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.x = 200.0f;
		rect.m_dimension.y = 20.0f;

		DoSliderEx(rect, percent, sliderStyle);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoSliderEx(const UIRect& rect, Float32* percent, const UISliderStyle& sliderStyle)
	{
		bool mouseInside = rect.isContain(UIVector2{ MainUIState.mouseX, MainUIState.mouseY });
		UInt32 _id = GetUIIDFromPointer(percent);

		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside)
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
		}

		if (MainUIState.activeItem.id == _id)
		{
			*percent = (MainUIState.mouseX - rect.m_pos.x) * 1.0f / rect.m_dimension.x;
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
		buttonRect.m_pos.x = rect.m_pos.x + ((rect.m_dimension.x - buttonRect.m_dimension.x) * *percent);
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
	}

	void UI::DoDropDown(Int32* selectedIdx, const UIChar** textOptions, Int32 optionCount, const UIDropdownStyle& style /*= DefaultDropdownStyle*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.x = style.dropdownButtonStyle.fontStyle.font->calculateTextLength(textOptions[*selectedIdx], style.dropdownButtonStyle.fontStyle.fontScale) + 60.0f;
		rect.m_dimension.y = style.dropdownButtonStyle.fontStyle.font->calculateTextHeight(style.dropdownButtonStyle.fontStyle.fontScale) + 20.0f;

		DoDropDownEx(rect, selectedIdx, textOptions, optionCount, style);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoDropDownEx(const UIRect& rect, Int32* selectedIdx, const UIChar** textOptions, Int32 optionCount, const UIDropdownStyle& style /*= DefaultDropdownStyle*/)
	{
		bool bPressed = false;
		UInt32 _id = GetUIIDFromPointer(selectedIdx);

		UIRect selectionRect = rect;
		if (cacheId == _id)
		{
			selectionRect.m_dimension.y += rect.m_dimension.y * optionCount;
		}

		bool mouseInside = selectionRect.isContain(UIVector2(MainUIState.mouseX, MainUIState.mouseY));

		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside)
		{
			MainUIState.hotItem.id = _id;
			if (MainUIState.activeItem.id == _id) 
			{ 
				MainUIState.activeItem.id = 0; bPressed = true; 
				if (cacheId != _id)
				{
					cacheId = _id;
				}
				else
				{
					cacheId = 0;
				}
			}
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) 
			{ 
				MainUIState.activeItem.id = 0;
			}
			else if (MainUIState.activeItem.id != -1 && cacheId == _id)
			{
				cacheId = 0;
			}
		}

		UIStyle uiStyle;
		if (MainUIState.activeItem.id == _id)
		{
			uiStyle = style.dropdownButtonStyle.down;
		}
		else if(MainUIState.hotItem.id == _id)
		{
			uiStyle = style.dropdownButtonStyle.hover;
		}
		else
		{
			uiStyle = style.dropdownButtonStyle.up;
		}

		UIRect textRect = rect;
		if (uiStyle.texture)
		{
			MainUIState.drawer->DrawTexture(rect, uiStyle.texture, uiStyle.fillColor, uiStyle.textureScale, uiStyle.textureOffset);
			textRect.m_pos.x += uiStyle.textureOffset.z * rect.m_dimension.x;
			textRect.m_pos.y += uiStyle.textureOffset.x * rect.m_dimension.y;
			textRect.m_dimension.x -= (uiStyle.textureOffset.z + uiStyle.textureOffset.w) * rect.m_dimension.x;
			textRect.m_dimension.y -= (uiStyle.textureOffset.x + uiStyle.textureOffset.y) * rect.m_dimension.y;
		}
		else
		{
			MainUIState.drawer->DrawRect(rect, uiStyle.fillColor);
		}

		DrawTextInRect(textRect, textOptions[*selectedIdx], UIVector4(1.0f), TEXT_LEFT, TEXT_V_CENTER, style.dropdownButtonStyle.fontStyle.fontScale, style.dropdownButtonStyle.fontStyle.font);

		if (cacheId == _id)
		{
			MainUIState.drawer->SetLayer(1); // Draw on top of everything

			// Open selector
			selectionRect.m_pos.y += rect.m_dimension.y;
			selectionRect.m_dimension.y = rect.m_dimension.y * optionCount;
			if (style.selectorStyle.texture)
			{
				MainUIState.drawer->DrawTexture(selectionRect, style.selectorStyle.texture, style.selectorStyle.fillColor, style.selectorStyle.textureScale, style.selectorStyle.textureOffset);
			}
			else
			{
				MainUIState.drawer->DrawRect(selectionRect, style.selectorStyle.fillColor);
			}

			// Drawing Text
			UIRect selRect = rect;
			UIRect selTextRect = rect;
			selTextRect.m_pos.y += rect.m_dimension.y;
			selTextRect.m_pos.x = textRect.m_pos.x;
			UIVector2 mousePos(MainUIState.mouseX, MainUIState.mouseY);
			for (int i = 0; i < optionCount; i++)
			{
				if (selTextRect.isContain(mousePos))
				{
					selRect.m_pos.y = selTextRect.m_pos.y;
					MainUIState.drawer->DrawRect(selRect, style.selectorHoverStyle.fillColor);
					if (MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
					{
						cacheId = 0;
						*selectedIdx = i;
					}
				}

				DrawTextInRect(selTextRect, textOptions[i], UIVector4(1.0f), TEXT_LEFT, TEXT_V_CENTER, DefaultDropdownStyle.selectorFontStyle.fontScale, DefaultDropdownStyle.selectorFontStyle.font);
				selTextRect.m_pos.y += rect.m_dimension.y;
			}

			MainUIState.drawer->SetLayer(0);
		}
	}

	void UI::DoTextInput(UIChar* bufferChar, Int32 bufferCount, const UITextInputStyle& style /*= DefaultTextInputStyle*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.x = rect.m_dimension.x;
		rect.m_dimension.y = style.fontStyle.font->calculateTextHeight(style.fontStyle.fontScale) + 15.0f;

		DoTextInputEx(rect, bufferChar, bufferCount, style);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoTextInputEx(const UIRect& rect, UIChar* bufferChar, Int32 bufferCount, const UITextInputStyle& style /*= DefaultTextInputStyle*/)
	{
		bool mouseInside = rect.isContain(UIVector2(MainUIState.mouseX, MainUIState.mouseY));
		UIRect textRect(UIVector2(rect.m_pos.x + 10, rect.m_pos.y), UIVector2(rect.m_dimension.x - 20, rect.m_dimension.y));
		Int32 textInputScrollPos = 0;

		UInt32 _id = GetUIIDFromPointer(bufferChar);

		static Int32 lastMaxLength = 100000;

		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN)
		{
			if (MainUIState.activeItem.id != _id)
			{
				// Set Main UI State to use this
				MainUIState.lastTextInput.id = _id;
				MainUIState.textInputBuffer = bufferChar;
				MainUIState.textInputCurrentPos = TextLength(bufferChar);
				MainUIState.textInputLength = bufferCount;
				MainUIState.textInputScrollPos = 0;
				MainUIState.textInputFilterChar = nullptr;
				lastMaxLength = 100000;
			}
			else
			{
				// Update text cursor
				MainUIState.textInputCurrentPos = MainUIState.textInputScrollPos + style.fontStyle.font->calculatePositionAtLength(bufferChar + MainUIState.textInputScrollPos * sizeof(UIChar), MainUIState.mouseX - textRect.m_pos.x, style.fontStyle.fontScale);
			}
			MainUIState.activeItem.id = _id;
		}
		else if (mouseInside && MainUIState.hotItem.id != _id)
		{
			MainUIState.hotItem.id = _id;
		}
		else if(!mouseInside && MainUIState.hotItem.id == _id)
		{
			MainUIState.hotItem.id = 0;
		}

		const UIStyle* rectStyle = nullptr;
		if (MainUIState.activeItem.id == _id || MainUIState.hotItem.id == _id)
		{
			rectStyle = &style.activeStyle;
		}
		else
		{
			rectStyle = &style.defaultStyle;
		}

		if (MainUIState.activeItem.id == _id)
		{
			textInputScrollPos = MainUIState.textInputScrollPos;
			Float32 textLength = style.fontStyle.font->calculateTextLength(bufferChar + textInputScrollPos * sizeof(UIChar), style.fontStyle.fontScale);
			if (textLength > textRect.m_dimension.x)
			{
				MainUIState.textInputMaxScroll = style.fontStyle.font->calculatePositionAtLength(bufferChar + textInputScrollPos * sizeof(UIChar), textRect.m_dimension.x, style.fontStyle.fontScale) - 1;
				lastMaxLength = MainUIState.textInputMaxScroll;
			}
			else
			{
				MainUIState.textInputMaxScroll = lastMaxLength; // style.fontStyle.font->calculatePositionAtLength(bufferChar + textInputScrollPos * sizeof(UIChar), textRect.m_dimension.x, style.fontStyle.fontScale);
			}
		}

		if (rectStyle->texture)
		{
			MainUIState.drawer->DrawTexture(rect, rectStyle->texture, rectStyle->fillColor, rectStyle->textureScale, rectStyle->textureOffset);
		}
		else
		{
			MainUIState.drawer->DrawRect(rect, rectStyle->fillColor);
		}


		if (bufferChar[0] != 0)
		{
			DrawTextInRect(textRect, bufferChar + textInputScrollPos * sizeof(UIChar), UIVector4(1.0f), ZE::TEXT_LEFT, ZE::TEXT_V_CENTER, style.fontStyle.fontScale, style.fontStyle.font);
		}

		if (MainUIState.activeItem.id == _id)
		{
			static Float32 timer = 0.0f;
			timer += MainUIState.mainTimer.GetDeltaMS() / 1000.0f;
			if (int(timer * 50.0f) % 2 == 0)
			{
				static UIRect blinkRect;
				blinkRect.m_dimension = { 2.0f, style.fontStyle.font->calculateTextHeight(style.fontStyle.fontScale) };
				blinkRect.m_pos.x = textRect.m_pos.x + style.fontStyle.font->calculateNTextLength(bufferChar + textInputScrollPos * sizeof(UIChar), MainUIState.textInputCurrentPos - textInputScrollPos, style.fontStyle.fontScale);
				blinkRect.m_pos.y = textRect.m_pos.y + (textRect.m_dimension.y - blinkRect.m_dimension.y) * 0.5f;
				MainUIState.drawer->DrawRect(blinkRect, UIVector4(1.0f));
			}
		}
	}

	void UI::DoNumberStepper(Float32* number, Float32 step, bool asInt /*= false*/, const UIFontStyle& textStyle /*= DefaultFontStyle*/, const UIButtonStyle& buttonStyle /*= DefaultButtonStyle*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.y = textStyle.font->calculateTextHeight(textStyle.fontScale) + 20.0f;

		DoNumberStepperEx(rect, number, step, asInt, textStyle);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoNumberStepperEx(const UIRect& rect, Float32* number, Float32 step, bool asInt /*= false*/, const UIFontStyle& textStyle /*= DefaultFontStyle*/, const UIButtonStyle& buttonStyle /*= DefaultButtonStyle*/)
	{
		UIChar charBuffer[256];
		StringHelper::NumberToString(*number, charBuffer, 256, asInt);
		DrawTextInRect(rect, charBuffer, UIVector4(1.0f), TEXT_CENTER, TEXT_V_CENTER, textStyle.fontScale, textStyle.font);
		
		UInt32 _id = GetUIIDFromPointer(number);

		StackIDs.push_back(_id);

		UIRect buttonRect;
		buttonRect.m_dimension.x = 35.0f;
		buttonRect.m_dimension.y = rect.m_dimension.y;
		buttonRect.m_pos = rect.m_pos;
		if (DoButtonEx("<", buttonRect, buttonStyle))
		{
			*number -= step;
		}

		buttonRect.m_pos.x = rect.m_pos.x + rect.m_dimension.x - buttonRect.m_dimension.x;
		if (DoButtonEx(">", buttonRect, buttonStyle))
		{
			*number += step;
		}

		StackIDs.pop_back();
	}

	void UI::DoNumberInput(Float32* number, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.y = style.fontStyle.font->calculateTextHeight(style.fontStyle.fontScale) + 20.0f;

		DoNumberInputEx(rect, number, style, asInt);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoNumberInputEx(const UIRect& rect, Float32* number, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		static UIChar charBuffer[256];
		UInt32 _id = GetUIIDFromPointer(number);
		UIChar* bufferChar = MainUIState.activeItem.id == _id ? MainUIState.textTempBuffer : charBuffer;
		
		bool mouseInside = rect.isContain(UIVector2(MainUIState.mouseX, MainUIState.mouseY));
		UIRect textRect(UIVector2(rect.m_pos.x + 10, rect.m_pos.y), UIVector2(rect.m_dimension.x - 20, rect.m_dimension.y));
		Int32 textInputScrollPos = 0;

		static Int32 lastMaxLength = 100000;

		if (mouseInside && MainUIState.mouseDown == EButtonState::BUTTON_DOWN) // Set this as an active if clicked
		{
			if (MainUIState.activeItem.id != _id)
			{
				// Set Main UI State to use this
				MainUIState.lastTextInput.id = _id;
				MainUIState.textInputBuffer = MainUIState.textTempBuffer;
				MainUIState.textInputCurrentPos = TextLength(MainUIState.textTempBuffer);
				MainUIState.textInputLength = TEXT_TEMP_LENGTH;
				MainUIState.textInputScrollPos = 0;
				MainUIState.textInputFilterChar = StringHelper::NumberFilterChar;
				lastMaxLength = 100000;
			}
			else
			{
				// Update text cursor
				MainUIState.textInputCurrentPos = MainUIState.textInputScrollPos + style.fontStyle.font->calculatePositionAtLength(MainUIState.textTempBuffer + MainUIState.textInputScrollPos * sizeof(UIChar), MainUIState.mouseX - textRect.m_pos.x, style.fontStyle.fontScale);
			}
			MainUIState.activeItem.id = _id;

			if (MainUIState.lastActiveItem.id == 0)
			{
				StringHelper::NumberToString(*number, MainUIState.textTempBuffer, TEXT_TEMP_LENGTH, asInt);
				MainUIState.lastActiveItem.id = _id;
			}
			else if (MainUIState.lastActiveItem.id != _id)
			{
				StringHelper::NumberToString(*number, charBuffer, 256, asInt);
			}
		}
		else if (MainUIState.activeItem.id == _id) // when this active
		{
			if (MainUIState.lastActiveItem.id == 0)
			{
				StringHelper::NumberToString(*number, MainUIState.textTempBuffer, TEXT_TEMP_LENGTH, asInt);
				MainUIState.lastActiveItem.id = _id;
			}
			else if (MainUIState.lastActiveItem.id != _id)
			{
				StringHelper::NumberToString(*number, charBuffer, 256, asInt);
			}

			if (mouseInside && MainUIState.hotItem.id != _id)
			{
				MainUIState.hotItem.id = _id;
			}
		}
		else if (MainUIState.lastActiveItem.id == _id && MainUIState.activeItem.id != _id) // This was active and then not active, update the value
		{
			MainUIState.lastActiveItem.id = 0;
			*number = StringHelper::StringToNumber(MainUIState.textTempBuffer);
			StringHelper::NumberToString(*number, charBuffer, 256, asInt);
		}
		else if (mouseInside && MainUIState.hotItem.id != _id)
		{
			MainUIState.hotItem.id = _id;
			StringHelper::NumberToString(*number, charBuffer, 256, asInt);
		}
		else if (!mouseInside && MainUIState.hotItem.id == _id)
		{
			MainUIState.hotItem.id = 0;
			StringHelper::NumberToString(*number, charBuffer, 256, asInt);
		}
		else
		{
			StringHelper::NumberToString(*number, charBuffer, 256, asInt);
		}

		const UIStyle* rectStyle = nullptr;
		if (MainUIState.activeItem.id == _id || MainUIState.hotItem.id == _id)
		{
			rectStyle = &style.activeStyle;
		}
		else
		{
			rectStyle = &style.defaultStyle;
		}

		if (MainUIState.activeItem.id == _id)
		{
			textInputScrollPos = MainUIState.textInputScrollPos;
			Float32 textLength = style.fontStyle.font->calculateTextLength(bufferChar + textInputScrollPos * sizeof(UIChar), style.fontStyle.fontScale);
			if (textLength > textRect.m_dimension.x)
			{
				MainUIState.textInputMaxScroll = style.fontStyle.font->calculatePositionAtLength(bufferChar + textInputScrollPos * sizeof(UIChar), textRect.m_dimension.x, style.fontStyle.fontScale) - 1;
				lastMaxLength = MainUIState.textInputMaxScroll;
			}
			else
			{
				MainUIState.textInputMaxScroll = lastMaxLength; // style.fontStyle.font->calculatePositionAtLength(bufferChar + textInputScrollPos * sizeof(UIChar), textRect.m_dimension.x, style.fontStyle.fontScale);
			}
		}

		if (rectStyle->texture)
		{
			MainUIState.drawer->DrawTexture(rect, rectStyle->texture, rectStyle->fillColor, rectStyle->textureScale, rectStyle->textureOffset);
		}
		else
		{
			MainUIState.drawer->DrawRect(rect, rectStyle->fillColor);
		}


		if (bufferChar[0] != 0)
		{
			DrawTextInRect(textRect, bufferChar + textInputScrollPos * sizeof(UIChar), UIVector4(1.0f), ZE::TEXT_LEFT, ZE::TEXT_V_CENTER, style.fontStyle.fontScale, style.fontStyle.font);
		}

		if (MainUIState.activeItem.id == _id)
		{
			static Float32 timer = 0.0f;
			timer += MainUIState.mainTimer.GetDeltaMS() / 1000.0f;
			if (int(timer * 50.0f) % 2 == 0)
			{
				static UIRect blinkRect;
				blinkRect.m_dimension = { 2.0f, style.fontStyle.font->calculateTextHeight(style.fontStyle.fontScale) };
				blinkRect.m_pos.x = textRect.m_pos.x + style.fontStyle.font->calculateNTextLength(bufferChar + textInputScrollPos * sizeof(UIChar), MainUIState.textInputCurrentPos - textInputScrollPos, style.fontStyle.fontScale);
				blinkRect.m_pos.y = textRect.m_pos.y + (textRect.m_dimension.y - blinkRect.m_dimension.y) * 0.5f;
				MainUIState.drawer->DrawRect(blinkRect, UIVector4(1.0f));
			}
		}
	}

	void UI::DoVector2Input(Float32* vec2, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.y = style.fontStyle.font->calculateTextHeight(style.fontStyle.fontScale) + 20.0f;

		DoVector2InputEx(rect, vec2, style, asInt);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoVector2InputEx(const UIRect& rect, Float32* vec2, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		const unsigned int nCount = 2;
		float elementWidth = rect.m_dimension.x / nCount ;
		UIRect elementRect = rect;
		elementRect.m_dimension.x = elementWidth - 5.0f;
		for (unsigned int i = 0; i < nCount; i++)
		{
			elementRect.m_pos.x = rect.m_pos.x + i * elementWidth;
			DoNumberInputEx(elementRect, &vec2[i], style, asInt);
		}
	}

	void UI::DoVector3Input(Float32* vec3, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.y = style.fontStyle.font->calculateTextHeight(style.fontStyle.fontScale) + 20.0f;

		DoVector3InputEx(rect, vec3, style, asInt);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoVector3InputEx(const UIRect& rect, Float32* vec3, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		const unsigned int nCount = 3;
		float elementWidth = rect.m_dimension.x / nCount;
		UIRect elementRect = rect;
		elementRect.m_dimension.x = elementWidth - 5.0f;
		for (unsigned int i = 0; i < nCount; i++)
		{
			elementRect.m_pos.x = rect.m_pos.x + i * elementWidth;
			DoNumberInputEx(elementRect, &vec3[i], style, asInt);
		}
	}

	void UI::DoVector4Input(Float32* vec4, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.y = style.fontStyle.font->calculateTextHeight(style.fontStyle.fontScale) + 20.0f;

		DoVector4InputEx(rect, vec4, style, asInt);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DoVector4InputEx(const UIRect& rect, Float32* vec4, const UITextInputStyle& style /*= DefaultTextInputStyle*/, bool asInt /*= false*/)
	{
		const unsigned int nCount = 4;
		float elementWidth = rect.m_dimension.x / nCount;
		UIRect elementRect = rect;
		elementRect.m_dimension.x = elementWidth - 5.0f;
		for (unsigned int i = 0; i < nCount; i++)
		{
			elementRect.m_pos.x = rect.m_pos.x + i * elementWidth;
			DoNumberInputEx(elementRect, &vec4[i], style, asInt);
		}
	}

	bool UI::BeginPanel(const UIChar* panelLabel, const UIRect initialRect, const UIPanelStyle& style /*= DefaultPanelStyle*/)
	{
		UInt32 _id = GetUIIDFromString(panelLabel);
		
		StackIDs.push_back(_id);

		if (!(HashMapHas(MainUIState.panelStates, _id)))
		{
			UIPanelState panelState;
			panelState.posAndDimension = initialRect;
			MainUIState.panelStates[_id] = panelState;
		}

		UIPanelState& panelState = MainUIState.panelStates[_id];

		UIRect headerRect;
		headerRect.m_pos = panelState.posAndDimension.m_pos + UIVector2{ 10.0f, 0.0f };
		headerRect.m_dimension.x = panelState.posAndDimension.m_dimension.x - 20.0f;
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
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
		}
		else
		{
			if (MainUIState.hotItem.id == _id) { MainUIState.hotItem.id = 0; }
			if (MainUIState.activeItem.id == _id) { MainUIState.activeItem.id = 0; }
		}

		if (MainUIState.activeItem.id == _id && !bJustActive)
		{
			UIVector2 delta{ MainUIState.mouseDeltaX, MainUIState.mouseDeltaY };
			panelState.posAndDimension.m_pos = panelState.posAndDimension.m_pos + delta;
			headerRect.m_pos = headerRect.m_pos + delta;
		}

		if (style.panel.texture)
		{
			MainUIState.drawer->DrawTexture(panelState.posAndDimension, style.panel.texture, style.panel.fillColor, style.panel.textureScale, style.panel.textureOffset);
		}
		else
		{
			MainUIState.drawer->DrawRect(panelState.posAndDimension, style.panel.fillColor);
		}

		UIFont* font = style.headerFontStyle.font ? style.headerFontStyle.font : DefaultFont;
		DrawTextInRect(headerRect, panelLabel, UIVector4(1.0f), TEXT_LEFT, TEXT_V_CENTER, style.headerFontStyle.fontScale, font);

		MainUIState.drawPosDimensionStack.push_back(MainUIState.drawPosDimension);

		MainUIState.drawPosDimension = panelState.posAndDimension;
		MainUIState.drawPosDimension.m_pos.x += 10.0f;
		MainUIState.drawPosDimension.m_pos.y += style.headerHeight + 10.0f;
		MainUIState.drawPosDimension.m_dimension.x -= 20.0f;

		MainUIState.drawDirectionStack.push_back(MainUIState.drawDirection);
		MainUIState.drawDirection = UIVector2(0.0f, 1.0f);

		return true;
	}

	void UI::EndPanel()
	{
		UInt32 parentId = StackIDs.back();
		UIPanelState& panelState = MainUIState.panelStates[parentId]; 
		panelState.posAndDimension.m_dimension.y = MainUIState.drawPosDimension.m_pos.y - panelState.posAndDimension.m_pos.y + 10.0f;

		UIRect newRect = MainUIState.drawPosDimensionStack.back();
		MainUIState.drawPosDimension = newRect;
		MainUIState.drawPosDimensionStack.pop_back();

		MainUIState.drawDirection = MainUIState.drawDirectionStack.back();
		MainUIState.drawDirectionStack.pop_back();

		StackIDs.pop_back();
	}

	void UI::DoText(const UIChar* text, const UIVector4& fillColor /*= UIVector4(1.0f)*/, const UIFontStyle& fontStyle /*= DefaultFontStyle*/)
	{
		DrawMultiLineText(text, fillColor, ZE::TEXT_LEFT, ZE::TEXT_V_TOP, fontStyle.fontScale, fontStyle.font);
	}

	void UI::DrawTextInPos(UIVector2& pos, const UIChar* text, const UIVector4& fillColor, UIFont* font, Float32 scale)
	{
		MainUIState.drawer->DrawText(pos, fillColor, font, text, scale);
	}

	void UI::DrawTextInRect(const UIRect& rect, const UIChar* text, UIVector4& fillColor, ETextAlign textAlign /*= TEXT_LEFT*/, ETextVerticalAlign vAlign /*= TEXT_V_CENTER*/, Float32 scale /*= 1.0f*/, UIFont* font)
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

		MainUIState.drawer->DrawText(actualPos, fillColor, font, text, scale, false, 0.0f, TEXT_LEFT, rect.m_dimension);
	}

	void UI::DrawMultiLineText(const UIChar* text, const UIVector4& fillColor, ETextAlign textAlign /*= TEXT_LEFT*/, ETextVerticalAlign vAlign /*= TEXT_V_TOP*/, Float32 scale /*= 1.0f*/, UIFont* font /*= DefaultFont*/)
	{
		UIVector2 textDimension;
		textDimension.x = MainUIState.drawPosDimension.m_dimension.x;
		textDimension.y = font->calculateTextHeight(scale);

		UIVector2 actualPos = MainUIState.drawPosDimension.m_pos;

		switch (vAlign)
		{
		case ZE::TEXT_V_BOTTOM:
			actualPos.y += (MainUIState.drawPosDimension.m_dimension.y - textDimension.y);
			break;
		case ZE::TEXT_V_CENTER:
			actualPos.y += (MainUIState.drawPosDimension.m_dimension.y - textDimension.y) / 2;
			break;
		}

		Int32 lineCount = 0;
		MainUIState.drawer->DrawText(actualPos, fillColor, font, text, scale, true, MainUIState.drawPosDimension.m_dimension.x, textAlign, MainUIState.drawPosDimension.m_dimension, &lineCount);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * UIVector2(0.0f, lineCount * font->calculateTextHeight(scale)) + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DrawMultiLineText(const UIVector2& dimension, const UIChar* text, const UIVector4& fillColor, ETextAlign textAlign /*= TEXT_LEFT*/, ETextVerticalAlign vAlign /*= TEXT_V_TOP*/, Float32 scale /*= 1.0f*/, UIFont* font /*= DefaultFont*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension = dimension;

		DrawMultiLineTextEx(rect, text, fillColor, textAlign, vAlign, scale, font);

		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DrawMultiLineTextEx(const UIRect& rect, const UIChar* text, const UIVector4& fillColor, ETextAlign textAlign /*= TEXT_V_CENTER*/, ETextVerticalAlign vAlign /*= TEXT_V_TOP*/, Float32 scale /*= 1.0f*/, UIFont* font)
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

		MainUIState.drawer->DrawText(actualPos, fillColor, font, text, scale, true, rect.m_dimension.x, textAlign, rect.m_dimension);
	}

	void UI::DrawTexture(UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale /*= SCALE_IMAGE*/, const UIVector4& scaleOffset /*= UIVector4(0.0f)*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension.y = (Float32)texture->getHeight();
		MainUIState.drawer->DrawTexture(rect, texture, colorMultiplier, textureScale, scaleOffset);
		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DrawTexture(const UIVector2& dimension, UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale /*= SCALE_IMAGE*/, const UIVector4& scaleOffset /*= UIVector4(0.0f)*/)
	{
		static UIRect rect;
		rect = MainUIState.drawPosDimension;
		rect.m_dimension = dimension;
		MainUIState.drawer->DrawTexture(rect, texture, colorMultiplier, textureScale, scaleOffset);
		MainUIState.drawPosDimension.m_pos = MainUIState.drawPosDimension.m_pos + MainUIState.drawDirection * rect.m_dimension + UIVector2(5.0f) * MainUIState.drawDirection;
	}

	void UI::DrawTextureInPos(const UIVector2& pos, UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale, const UIVector4& scaleOffset)
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

	void UI::DrawTextureInPos(const UIRect& rect, UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale, const UIVector4& scaleOffset)
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

		drawItem->m_layer = m_currentLayer;
		Float32 depth = m_currentLayer * 0.5f + m_currentDepth;

		UIVector2 positions[4] = { rect.m_pos, 
								{ rect.m_pos.x + rect.m_dimension.x, rect.m_pos.y }, 
								{ rect.m_pos.x, rect.m_pos.y + rect.m_dimension.y }, 
								rect.m_pos + rect.m_dimension };
		UIVector2 texCoords[4] = { {0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f} };
		
		if (textureScale == ZE::SCALE_IMAGE)
		{
#if defined(ZUI_USE_RECT_INSTANCING)
			drawItem->m_instances.push_back(UIInstance{ rect.m_pos, depth, rect.m_dimension, 0.0f, fillColor, UIVector4{ 0.0f, 0.0f, 1.0f, 1.0f } });
#else
			drawItem->m_vertices.push_back(UIVertex{ positions[0], depth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[1], depth, texCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[3], depth, texCoords[3], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[0], depth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[3], depth, texCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ positions[2], depth, texCoords[2], fillColor });
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
			UIVector2 edgeRectDimRight{ smallRectDim2.x, rect.m_dimension.y - smallRectDim2.y - smallRectDim4.y };

			// Top Left Corner
			drawItem->m_instances.push_back(UIInstance{ rect.m_pos, depth, smallRectDim1, 0.0f, fillColor, UIVector4{ 0.0f, 0.0f, left, top } });

			// Bottom Left Corner
			drawItem->m_instances.push_back(UIInstance{ outerPositions[5], depth, smallRectDim3, 0.0f, fillColor, UIVector4{ outerTexCoords[5].x, outerTexCoords[5].y, left, bottom } });

			// Right Top Corner
			drawItem->m_instances.push_back(UIInstance{ outerPositions[2], depth, smallRectDim2, 0.0f, fillColor, UIVector4{ outerTexCoords[2].x, outerTexCoords[2].y, right, top } });

			// Right Bottom Corner
			drawItem->m_instances.push_back(UIInstance{ innerPositions[3], depth, smallRectDim4, 0.0f, fillColor, UIVector4{ innerTexCoords[3].x, innerTexCoords[3].y, right, bottom } });
			
			// Top Edge
			drawItem->m_instances.push_back(UIInstance{ outerPositions[0], depth, edgeRectDimTop, 0.0f, fillColor, UIVector4{ outerTexCoords[0].x, outerTexCoords[0].y, 1.0f - right - left, top } });
			
			// Bottom Edge
			drawItem->m_instances.push_back(UIInstance{ innerPositions[2], depth, edgeRectDimBottom, 0.0f, fillColor, UIVector4{ innerTexCoords[2].x, innerTexCoords[2].y, 1.0f - right - left, bottom } });
			
			// Left Edge
			drawItem->m_instances.push_back(UIInstance{ outerPositions[1], depth, edgeRectDimLeft, 0.0f, fillColor, UIVector4{ outerTexCoords[1].x, outerTexCoords[1].y, left, 1.0f - top - bottom } });

			// Right Edge
			drawItem->m_instances.push_back(UIInstance{ innerPositions[1], depth, edgeRectDimRight, 0.0f, fillColor, UIVector4{ innerTexCoords[1].x, innerTexCoords[1].y, right, 1.0f - top - bottom } });

#else
			// Top-Left Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[0], depth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[0], depth, outerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], depth, innerTexCoords[0], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[0], depth, texCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], depth, innerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[1], depth, outerTexCoords[1], fillColor });

			// Top-Right Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[1], depth, texCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[3], depth, outerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], depth, innerTexCoords[1], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[1], depth, texCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], depth, innerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[2], depth, outerTexCoords[2], fillColor });

			// Bottom-Left Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[2], depth, texCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[5], depth, outerTexCoords[5], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], depth, innerTexCoords[2], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[2], depth, texCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], depth, innerTexCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[4], depth, outerTexCoords[4], fillColor });

			// Bottom-Right Corner
			drawItem->m_vertices.push_back(UIVertex{ positions[3], depth, texCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[7], depth, outerTexCoords[7], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], depth, innerTexCoords[3], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ positions[3], depth, texCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], depth, innerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[6], depth, outerTexCoords[6], fillColor });

			// Top
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[0], depth, outerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[2], depth, outerTexCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], depth, innerTexCoords[1], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[0], depth, outerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], depth, innerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], depth, innerTexCoords[0], fillColor });

			// Bottom
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[4], depth, outerTexCoords[4], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], depth, innerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], depth, innerTexCoords[2], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[6], depth, outerTexCoords[6], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[4], depth, outerTexCoords[4], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], depth, innerTexCoords[3], fillColor });

			// Left
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[1], depth, outerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], depth, innerTexCoords[2], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[5], depth, outerTexCoords[5], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[1], depth, outerTexCoords[1], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], depth, innerTexCoords[0], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], depth, innerTexCoords[2], fillColor });

			// Right
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[3], depth, outerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ outerPositions[7], depth, outerTexCoords[7], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], depth, innerTexCoords[1], fillColor });

			drawItem->m_vertices.push_back(UIVertex{ outerPositions[7], depth, outerTexCoords[7], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], depth, innerTexCoords[3], fillColor });
			drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], depth, innerTexCoords[1], fillColor });
#endif
			if (textureScale == SCALE_9SCALE)
			{
#if defined(ZUI_USE_RECT_INSTANCING)
				drawItem->m_instances.push_back(UIInstance{ innerPositions[0], depth, { rect.m_dimension.x - smallRectDim1.x - smallRectDim2.x, rect.m_dimension.y - smallRectDim1.y - smallRectDim3.y }, 0.0f, fillColor, UIVector4{ innerTexCoords[0].x, innerTexCoords[0].y, 1.0f - left - right, 1.0f - top - bottom } });
#else
				// Center
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], depth, innerTexCoords[0], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[1], depth, innerTexCoords[1], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], depth, innerTexCoords[3], fillColor });

				drawItem->m_vertices.push_back(UIVertex{ innerPositions[0], depth, innerTexCoords[0], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[3], depth, innerTexCoords[3], fillColor });
				drawItem->m_vertices.push_back(UIVertex{ innerPositions[2], depth, innerTexCoords[2], fillColor });
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

		drawItem->m_layer = m_currentLayer;
		Float32 depth = m_currentLayer * 0.5f + m_currentDepth;

#if defined(ZUI_USE_RECT_INSTANCING)
		drawItem->m_instances.push_back(UIInstance{ rect.m_pos, depth, rect.m_dimension, rect.m_roundness, fillColor });
		drawItem->m_bUsingRectInstance = true;
#else
		UIVector2 positions[4] = { rect.m_pos,
		{ rect.m_pos.x + rect.m_dimension.x, rect.m_pos.y },
		{ rect.m_pos.x, rect.m_pos.y + rect.m_dimension.y },
			rect.m_pos + rect.m_dimension };
		UIVector2 texCoords[4] = { { 0.0f, 0.0f },{ 1.0f, 0.0f },{ 0.0f, 1.0f },{ 1.0f, 1.0f } };

		drawItem->m_vertices.push_back(UIVertex{ positions[0], depth, texCoords[0], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[1], depth, texCoords[1], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[3], depth, texCoords[3], fillColor });

		drawItem->m_vertices.push_back(UIVertex{ positions[0], depth, texCoords[0], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[3], depth, texCoords[3], fillColor });
		drawItem->m_vertices.push_back(UIVertex{ positions[2], depth, texCoords[2], fillColor });
		
		drawItem->m_roundness = rect.m_roundness;
		drawItem->m_shapeDimension = rect.m_dimension;
#endif
		m_currentDepth += m_step;
	}

	void UIDrawer::DrawShape(UIArray<UIVector2>& points, const UIVector4& fillColor)
	{

	}

	void UIDrawer::DrawText(UIVector2& pos, const UIVector4& fillColor, UIFont* font, const UIChar* text, Float32 scale /*= 1.0f*/, bool bWordWrap /*= false*/, Float32 maxWidth /*= 0*/, ETextAlign wrapTextAlign /*= TEXT_LEFT*/, const UIVector2& dim, Int32* lineCount)
	{
#if defined(ZUI_USE_FONT_INSTANCING) && !defined(ZUI_USE_SINGLE_TEXT_ONLY)
		UIDrawItem* drawItem = m_currentDrawList->getTextureInstanceDrawItem(font->getTextureHandle());
#elif defined(ZUI_USE_SINGLE_TEXT_ONLY) || !defined(ZUI_GROUP_PER_TEXTURE)
		UIDrawItem* drawItem = m_currentDrawList->getNextDrawItem();
		drawItem->m_textureHandle = font->getTextureHandle();
		drawItem->m_pos = pos;
		drawItem->m_bCrop = dim.x > 0.0f && dim.y > 0.0f;
		drawItem->m_shapeDimension = dim;
#else
		UIDrawItem* drawItem = m_currentDrawList->getTextureDrawItem(font->getTextureHandle());
#endif

		drawItem->m_layer = m_currentLayer;

		if (bWordWrap)
		{
#if defined(ZUI_USE_FONT_INSTANCING)
			font->generateWrapTextInstanceBuffer(text, drawItem->m_instances, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentLayer * 0.5f + m_currentDepth, fillColor, maxWidth, wrapTextAlign, lineCount);
#else
			font->generateWrapTextVertexBuffer(text, drawItem->m_vertices, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentLayer * 0.5f + m_currentDepth, fillColor, maxWidth, wrapTextAlign);
#endif
		}
		else
		{
#if defined(ZUI_USE_FONT_INSTANCING)
			font->generateInstanceBuffer(text, drawItem->m_instances, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentLayer * 0.5f + m_currentDepth, fillColor);
#else
			font->generateVertexBuffer(text, drawItem->m_vertices, pos + UIVector2{ 0, 1.0f * font->calculateTextHeight(scale) }, scale, m_currentLayer * 0.5f + m_currentDepth, fillColor);
#endif
		}

#if defined(ZUI_USE_SINGLE_TEXT_ONLY)
		drawItem->m_bUsingRectInstance = true;
#endif
		drawItem->m_bFont = true;
#if defined(ZUI_GROUP_PER_TEXTURE)
		drawItem->m_layer = 1;
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
		m_layer = 0;
		m_bCrop = false;
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

	ZE::UIVector2 operator*(const UIVector2& v1, const UIVector2& v2)
	{
		return UIVector2{ v1.x * v2.x, v1.y * v2.y };
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

	ZE::Float32 UIFont::calculateNTextLength(const UIChar* text, Int32 n, Float32 scale)
	{
		int index = 0;
		char c = text[index++];
		Float32 result = 0;
		while (c != '\0' && n--)
		{
			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			result += scale * m_charDesc[charIndex].Advance;
			c = text[index++];
		}

		return result;
	}

	ZE::Int32 UIFont::calculatePositionAtLength(const UIChar* text, Float32 length, Float32 scale)
	{
		Int32 index = 0;
		char c = text[index++];
		while (c != '\0' && length > 0)
		{
			int charIndex;
			HashMapHasAndAssign(m_charMap, c, 0, charIndex);
			length -= scale * m_charDesc[charIndex].Advance;
			c = text[index++];
		}

		return index-1;
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

	void UIFont::generateWrapTextInstanceBuffer(const UIChar* text, UIArray<UIInstance>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color, Float32 maxWidth, ETextAlign textAlign, Int32* lineCount)
	{
		UIArray<UInt32> charIndices;
		Float32 width;
		Int32 count = 0;
		Float32 x;
		Float32 y = -pos.y;
		if (lineCount) { *lineCount = 0; }

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
			if (lineCount) { (*lineCount)++; }
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

	void StringHelper::NumberToString(Float32 number, UIChar* bufferChar, Int32 bufferCount, bool asInt /*= false*/)
	{
		sprintf_s(bufferChar, bufferCount, asInt ? "%.0f" : "%.2f", number);
	}

	ZE::Float32 StringHelper::StringToNumber(UIChar* bufferChar)
	{
		Float32 result = 0.0f;

		Int32 idx = 0;
		Float32 divider = 1.0f;
		bool decimalFlag = false;

		while (bufferChar[idx] != 0)
		{
			if (bufferChar[idx] == '.')
			{
				decimalFlag = true;
				idx++;
				continue;
			}

			if (decimalFlag)
			{
				divider =  divider * 0.1f;
			}
			else
			{
				result *= 10.0f;
			}

			result += float(bufferChar[idx] - '0') * divider;

			idx++;
		}

		return result;
	}

	bool StringHelper::NumberFilterChar(UIChar keyChar)
	{
		return (keyChar >= '0' && keyChar <= '9') || keyChar == '.';
	}

}