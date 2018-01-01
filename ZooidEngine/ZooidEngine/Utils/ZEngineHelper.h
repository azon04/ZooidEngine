#ifndef __ZENGINE_HELPER_H__ 
#define __ZENGINE_HELPER_H__

#include "Macros.h"
#include "Debug.h"

#define ZEINFO(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZE_INFO: " ## textFormat ## "\r\n", __VA_ARGS__);

#define ZEWARNING(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZE_WARNING: " ## textFormat ## "\r\n", __VA_ARGS__);

#define ZEERROR(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZE_ERROR: " ## textFormat ## "\r\n", __VA_ARGS__);

#include <cassert>
#define ZASSERT(condition,textFormat, ...)\
	if(condition == false) \
	{ \
		ZEERROR(textFormat, __VA_ARGS__); \
		assert(condition); \
	}

#endif 
