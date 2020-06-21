#ifndef __ZE_KEYS_H__
#define __ZE_KEYS_H__

namespace ZE
{
	namespace KeyState
	{
		enum KeyStateEnum
		{
			Pressed,
			Released,
			Repeat
		};
	}

	// Keys
	// The values here are the same as the value in Win32 virtual key code: https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes
	namespace Key
	{
		enum KeyEnum
		{
			None				= 0x00,
			// Mouse button
			MouseLeftButton		= 0x01,
			MouseRightButton	= 0x02,
			Cancel				= 0x03,
			MouseMiddleButton	= 0x04,
			MouseXButton1		= 0x05,
			MouseXButton2		= 0x06,

			// Keyboard
			Backspace			= 0x08,
			Tab					= 0x09,
			Clear				= 0x0C,
			Return				= 0x0D,
			Shift				= 0x10,
			Ctrl				= 0x11,
			Alt					= 0x12,
			Pause				= 0x13,
			CapsLock			= 0x14,
			Esc					= 0x1B,

			Space				= 0x20,
			PageUp				= 0x21,
			PageDown			= 0x22,
			End					= 0x23,
			Home				= 0x24,
			LeftArrow           = 0x25,
			UpArrow             = 0x26,
			RightArrow          = 0x27,
			DownArrow           = 0x28,
			Select				= 0x29,
			Print				= 0x2A,
			Execute				= 0x2B,
			PrintScreen			= 0x2C,
			Insert				= 0x2D,
			Delete              = 0x2E,
			Help				= 0x2F,

			// Number
			Number0				= 0x30,
			Number1				= 0x31,
			Number2				= 0x32,
			Number3				= 0x33,
			Number4				= 0x34,
			Number5				= 0x35,
			Number6				= 0x36,
			Number7				= 0x37,
			Number8				= 0x38,
			Number9				= 0x39,

			// Alphabet
			A					= 0x41,
			B					= 0x42,
			C					= 0x43,
			D					= 0x44,
			E					= 0x45,
			F					= 0x46,
			G					= 0x47,
			H					= 0x48,
			I					= 0x49,
			J					= 0x4A,
			K					= 0x4B,
			L					= 0x4C,
			M					= 0x4D,
			N					= 0x4E,
			O					= 0x4F,
			P					= 0x50,
			Q					= 0x51,
			R					= 0x52,
			S					= 0x53,
			T					= 0x54,
			U					= 0x55,
			V					= 0x56,
			W					= 0x57,
			X					= 0x58,
			Y					= 0x59,
			Z					= 0x5A,

			LetWindowsKey       = 0x5B,
			RightWindowsKey     = 0x5C,
			AppKey				= 0x5D,
			SleepKey			= 0x5F,

			Numpad0				= 0x60,
			Numpad1				= 0x61,
			Numpad2				= 0x62,
			Numpad3				= 0x63,
			Numpad4				= 0x64,
			Numpad5				= 0x65,
			Numpad6				= 0x66,
			Numpad7				= 0x67,
			Numpad8				= 0x68,
			Numpad9				= 0x69,
			Multiply			= 0x6A,
			Add					= 0x6B,
			Separator			= 0x6C,
			Substract			= 0x6D,
			Decimal				= 0x6E,
			Divide				= 0x6F,

			F1					= 0x70,
			F2					= 0x71,
			F3					= 0x72,
			F4					= 0x73,
			F5					= 0x74,
			F6					= 0x75,
			F7					= 0x76,
			F8					= 0x77,
			F9					= 0x78,
			F10					= 0x79,
			F11					= 0x7A,
			F12					= 0x7B,
			F13					= 0x7C,
			F14					= 0x7D,
			F15					= 0x7E,
			F16					= 0x7F,
			F17					= 0x80,
			F18					= 0x81,
			F19					= 0x82,
			F20					= 0x83,
			F21					= 0x84,
			F22					= 0x85,
			F23					= 0x86,
			F24					= 0x87,

			NumLock				= 0x90,
			ScrollLock          = 0x91,

			LeftShift			= 0xA0,
			RightShift			= 0xA1,
			LeftCtrl			= 0xA2,
			RightCtrl			= 0xA3,
			LeftMenu			= 0xA4,
			RightMenu			= 0xA5,

			Semicolon			= 0xBA,   // ';:' for US
			Plus				= 0xBB,   // '+' any country
			Comma				= 0xBC,   // ',' any country
			Minus				= 0xBD,   // '-' any country
			Period				= 0xBE,   // '.' any country
			Backslash			= 0xBF,   // '/?' for US
			Tilde				= 0xC0,   // '`~' for US
			OpenBracket			= 0xDB,  //  '[{' for US
			Slash				= 0xDC,  //  '\|' for US
			CloseBracket        = 0xDD, //  ']}' for US
			Quote				= 0xDE,  //  ''"' for US
		};
	}
}
#endif
