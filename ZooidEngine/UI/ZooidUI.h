#ifndef __ZE_UI_H__
#define __ZE_UI_H__

#include "ZooidUI_Config.h"

namespace ZE
{
	// Forward Declaration
	class UIRenderer;
	class UIDrawer;

	struct UI_ID
	{
		Int32 owner = -1;
		Int32 id = -1;
	};

	enum EButtonState : UInt8
	{
		BUTTON_UP = 0,
		BUTTON_DOWN = 1
	};
	
	enum ETextAlign : UInt8
	{
		TEXT_LEFT,
		TEXT_CENTER,
		TEXT_RIGHT
	};

	enum ETextVerticalAlign : UInt8
	{
		TEXT_V_TOP,
		TEXT_V_BOTTOM,
		TEXT_V_CENTER
	};

	enum ETextureScale : UInt8
	{
		SCALE_IMAGE,
		SCALE_9SCALE,
		SCALE_BORDER
	};

	class UIState
	{
	public:
		UI_ID hotItem;
		UI_ID activeItem;

		Float32 mouseX;
		Float32 mouseY;

		Float32 mouseDeltaX;
		Float32 mouseDeltaY;

		EButtonState mouseDown;

		UIRenderer* renderer;
		UIDrawer* drawer;
	};

	struct UIVector2
	{
		Float32 x;
		Float32 y;

		UIVector2()
		{
			x = y = 0.0f;
		}

		UIVector2(Float32 v)
		{
			x = y = v;
		}

		UIVector2(Float32 x, Float32 y)
		{
			this->x = x;
			this->y = y;
		}
	};

	// Vector2 Operation
	UIVector2 operator+(const UIVector2& v1, const UIVector2& v2);

	UIVector2 operator-(const UIVector2& v1, const UIVector2& v2);

	struct UIVector4
	{		
		union
		{
			struct  
			{
				Float32 x;
				Float32 y;
				Float32 z;
				Float32 w;
			};

			struct
			{
				Float32 r;
				Float32 g;
				Float32 b;
				Float32 a;
			};
		};
		
		UIVector4() 
		{
			x = y = z = w = 0.0f;
		}

		UIVector4(Float32 v)
		{
			x = y = z = w = v;
		}

		UIVector4(Float32 x, Float32 y, Float32 z, Float32 w)
		{
			this->x = x;
			this->y = y;
			this->z = z;
			this->w = w;
		}
	};

	class UIShape
	{
	public:
		UIShape() {}
		UIShape(const UIVector2& pos)
			: m_pos(pos)
		{ }

		UIVector2 m_pos;

		virtual bool isContain(const UIVector2& pos) const = 0;
	};

	class UIRect : public UIShape
	{
	public:
		UIRect() {}
		UIRect(const UIVector2& pos, const UIVector2& dimension, Float32 roundness = 0.0f)
			: UIShape(pos),
			m_dimension(dimension),
			m_roundness(roundness)
		{

		}

		UIVector2 m_dimension;
		Float32 m_roundness = 0.0f;

		virtual bool isContain(const UIVector2& pos) const override;
	};

	struct UIVertex
	{
		UIVector2 pos;
		Float32 depth;
		UIVector2 texCoord;
		UIVector4 color;
	};

	struct UIInstance
	{
		UIVector2 pos;
		Float32 depth;
		UIVector2 dimension;
		Float32 radius;
		UIVector4 color;
		UIVector4 uvDim;

		UIInstance() {}
		
		UIInstance(const UIVector2& pos, Float32 depth, const UIVector2& dimension, Float32 radius, const UIVector4& color, const UIVector4& uvDim)
		{
			this->pos = pos;
			this->depth = depth;
			this->dimension = dimension;
			this->radius = radius;
			this->color = color;
			this->uvDim = uvDim;
		}

		UIInstance(const UIVector2& pos, Float32 depth, const UIVector2& dimension, Float32 radius, const UIVector4& color)
		{
			this->pos = pos;
			this->depth = depth;
			this->dimension = dimension;
			this->radius = radius;
			this->color = color;
		}
	};

	class UIDrawItem
	{
		friend class UIDrawer;
		friend class UIDrawList;

	public:
		UIArray<UIVertex>& getVertices();
		UIArray<UIInstance>& getInstances();

		UInt32 getTextureHandle() const { return m_textureHandle; }

		bool isFont() const { return m_bFont; }
		bool isUsingRectInstance() const { return m_bUsingRectInstance; }

		UIVector2 getDimension() const { return m_shapeDimension; }
		Float32 getRoundness() const { return m_roundness; }
		UInt32 getPass() const { return m_pass; }

		void reset();

	protected:
		UIArray<UIVertex> m_vertices;
		UIArray<UIInstance> m_instances;

		UInt32 m_textureHandle = 0;
		Float32 m_roundness = 0.0f;
		UIVector2 m_shapeDimension{ 0, 0 };

		bool m_bFont = false;
		bool m_bUsingRectInstance = false;
		UInt32 m_pass = 0;
	};

	class UIDrawList
	{
		friend class UIDrawer;
	public:

		UIDrawList() {}
		virtual ~UIDrawList() { freeDrawItems(); }

		Int32 itemCount() const;
		UIDrawItem* getDrawItem(int _index) const;
		UIDrawItem* getNextDrawItem();
		void reset();

		void freeDrawItems();

		UIDrawItem* getTextureInstanceDrawItem(UInt32 _index);
		UIDrawItem* getTextureDrawItem(UInt32 textureHandle);

	protected:
		static const UInt32 maxInstanceNumber = 2;
		
		UIHashMap<UInt32, UInt32> m_textureToDrawItemMap;
		UIHashMap<UInt32, UInt32> m_textureToInstanceDrawItem;

		UIArray<UIDrawItem*> m_drawItems;

		UInt32 m_count = 0;

	};

	struct UIFontCharDesc
	{
		UIVector2 TexCoord;
		UIVector2 Bearing;
		UIVector2 Dimension;
		UInt32 Advance;
	};

	class UIFont
	{
	public:

		virtual ~UIFont() { release(); }
		static UIFont* loadFontFile(const UIChar* fontFilePath, UIRenderer* renderer, Int32 fontSizePx = 15);

		Float32 calculateTextLength(const UIChar* text, Float32 scale);
		Float32 calculateTextHeight(Float32 scale);
		Float32 calculateWordWrapTextHeight(const UIChar* text, Float32 scale, Int32 maxWidth);

		UInt32 getTextureHandle() const { return m_textureHandle; }
		
		void generateVertexBuffer(const UIChar* text, UIArray<UIVertex>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color);
		void generateWrapTextVertexBuffer(const UIChar* text, UIArray<UIVertex>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color, Float32 maxWidth, ETextAlign textAlign);
		
		void generateInstanceBuffer(const UIChar* text, UIArray<UIInstance>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color);
		void generateWrapTextInstanceBuffer(const UIChar* text, UIArray<UIInstance>& result, const UIVector2 pos, Float32 scale, Float32 depth, UIVector4 color, Float32 maxWidth, ETextAlign textAlign);

		void release();

	private:
		bool generateWrapTextBuffer(const UIChar* text, UIArray<UInt32>& charIndices, Float32 scale, Float32 depth, Float32 maxWidth, Float32& width, Int32& count);
		Int32 calculateFirstLineWidth(const UIChar* text, Float32 scale, Int32 maxWidth);

	protected:
		UIArray<UIFontCharDesc> m_charDesc;
		UIHashMap<UInt32, UInt32> m_charMap;
		UInt32 m_fontSize;
		UInt32 m_fontAtlasSize;
		UInt32 m_textureHandle;
		UIRenderer* m_renderer;
	};

	class UITexture
	{
		friend class UITextureManager;
	public:

		Int32 getWidth() { return m_width; }
		Int32 getHeight() { return m_height; }

		UInt32 getTextureHandle() { return m_textureHandle; }

		void release();
		
	protected:
		Int32 m_width;
		Int32 m_height;
		UInt32 m_textureHandle;
		UIRenderer* m_renderer;
	};

	class UITextureManager
	{
	public:
		static void Init();
		static void Destroy();

		static UITexture* LoadTexture(const UIChar* fontFilePath, UIRenderer* renderer);

	private:
		static UITextureManager* s_instance;

		UIArray<UITexture*> m_textures;
	};

	class UIDrawer
	{
	public:
		UIDrawer() : 
			m_currentDrawList(&m_drawLists[0])
		{}

		void DrawTexture(const UIRect& rect, UITexture* texture, const UIVector4& fillColor, ETextureScale textureScale = SCALE_IMAGE, const UIVector4& scaleOffset = UIVector4(0.0f));
		void DrawRect(const UIRect& rect, const UIVector4& fillColor);
		void DrawText(UIVector2& pos, const UIVector4& fillColor, UIFont* font, const UIChar* text, Float32 scale = 1.0f, bool bWordWrap = false, Float32 maxWidth = 0, ETextAlign wrapTextAlign = TEXT_LEFT);
		void DrawShape(UIArray<UIVector2>& points, const UIVector4& fillColor);
		void Reset();
		void SwapBuffer();

		UIDrawList* getCurrentDrawList() { return m_currentDrawList; }

	protected:
		UIDrawList m_drawLists[2]; // Double Buffer
		UIDrawList* m_currentDrawList; // Pointer to current Drawlist
		Float32 m_currentDepth = 0.00001f;
		const Float32 m_step = 0.00001f;
	};

	// Font Style
	struct UIFontStyle
	{
		UIFont* font = nullptr;
		Float32 fontScale = 1.0f;
	};

	// Button Style
	struct UIStyle
	{
		UITexture* texture = nullptr;
		UIVector2 textureSize;
		ETextureScale textureScale = SCALE_IMAGE;
		UIVector4 textureOffset;
		UIVector4 fillColor;
	};

	struct UIButtonStyle
	{
		UIStyle up;
		UIStyle hover;
		UIStyle down;
		UIStyle selected;
		UIFontStyle fontStyle;
	};

	struct UISliderStyle
	{
		UIStyle slider;
		UIButtonStyle sliderButton;
	};

	struct UIPanelStyle
	{
		UIStyle panel;
		UIStyle panelClosedBG;
		Float32 headerHeight;
		UIStyle panelClosed;
		UIStyle panelOpened;
		UIFontStyle headerFontStyle;
	};

	// Functions to use to draw everything
	namespace UI
	{
		// UI State
		extern UIState MainUIState;

		// Default Styles
		extern UIButtonStyle DefaultButtonStyle;
		extern UIButtonStyle DefaultCheckBoxStyle;
		extern UIButtonStyle DefaultRadioBtnStyle;
		extern UISliderStyle DefaultSliderStyle;
		extern UIPanelStyle DefaultPanelStyle;
		extern UIFont* DefaultFont;

		// Getter
		UIState* GetUIState();

		// Function
		void Init(Int32 width, Int32 height);
		void Destroy();

		void BeginFrame();
		void EndFrame();

		void UpdateMouseState(Float32 mouseX, Float32 mouseY, EButtonState mouseDown);

		bool DoButton(Int32 _id, UIRect& rect, const UIButtonStyle& buttonStyle = DefaultButtonStyle);
		
		// Return True if Checked
		bool DoCheckBox(Int32 _id, const UIVector2& pos, const UIChar* text, bool bChecked, const UIButtonStyle& checkBoxStyle = DefaultCheckBoxStyle);

		// Return Selected Id if Checked
		Int32 DoRadioButton(Int32 _id, const UIVector2& pos, const UIChar* text, Int32 _selectedId, const UIButtonStyle& checkBoxStyle = DefaultRadioBtnStyle);

		// Panel
		void DoPanel(Int32 _id, const UIRect& panelRect, const UIChar* text, Float32 padding, UIVector2& contentPos, bool& bClosed, const UIPanelStyle& style = DefaultPanelStyle);
		
		// Draggable Panel
		void DoDragablePanel(Int32 _id, UIRect& panelRect, const UIChar* text, Float32 padding, UIVector2& contentPos, const UIPanelStyle& style = DefaultPanelStyle);
		
		// Slider
		Float32 DoSlider(Int32 _id, const UIRect& rect, Float32 percent, const UISliderStyle& sliderStyle = DefaultSliderStyle);

		void DrawTextInPos(Int32 _id, UIVector2& pos, const UIChar* text, const UIVector4& fillColor, UIFont* font = DefaultFont, Float32 scale = 1.0f);
		void DrawTextInRect(Int32 _id, UIRect& rect, const UIChar* text, UIVector4& fillColor, ETextAlign textAlign = TEXT_LEFT, ETextVerticalAlign vAlign = TEXT_V_CENTER, Float32 scale = 1.0f, UIFont* font = DefaultFont);
		void DrawMultiLineText(Int32 _id, const UIRect& rect, const UIChar* text, const UIVector4& fillColor, ETextAlign textAlign = TEXT_LEFT, ETextVerticalAlign vAlign = TEXT_V_TOP, Float32 scale = 1.0f, UIFont* font = DefaultFont);

		void DrawTexture(Int32 _id, const UIVector2& pos, UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale = SCALE_IMAGE, const UIVector4& scaleOffset = UIVector4(0.0f));

		void DrawTexture(Int32 _id, const UIRect& rect, UITexture* texture, const UIVector4& colorMultiplier, ETextureScale textureScale = SCALE_IMAGE, const UIVector4& scaleOffset = UIVector4(0.0f));

		void ProcessDrawList();

		// Texture Load
		UITexture* LoadTexture(const char* filePath);

		// Utils
		Float32 Lerp(Float32 start, Float32 next, Float32 alpha);

		// Platform Implementation
		namespace Platform
		{
			UIRenderer* CreateRenderer();
		}
	};

	
}
#endif
