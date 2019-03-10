#include "Debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <windows.h>

#include "DebugOptions.h"

namespace ZE
{
	DebugOptions gDebugOptions;

	void Debug::PrintDebugString(const char* textFormat, ...)
	{
		char szBuff[1024];
		va_list arg;
		va_start(arg, textFormat);
		_vsnprintf_s(szBuff, sizeof(szBuff), textFormat, arg);
		va_end(arg);

		OutputDebugStringA(szBuff);
	}
}
