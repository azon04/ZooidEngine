#include "Debug.h"
#include <stdio.h>
#include <stdarg.h>
#include <WinBase.h>

void ZE::Debug::PrintDebugString(const char* textFormat, ...)
{
	char szBuff[1024];
	va_list arg;
	va_start(arg, textFormat);
	_vsnprintf_s(szBuff, size(szBuff), textFormat, arg);
	va_end(arg);

	OutputDebugStringA(szBuff);
}
