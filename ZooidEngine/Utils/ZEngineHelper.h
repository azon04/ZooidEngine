#ifndef __ZENGINE_HELPER_H__ 
#define __ZENGINE_HELPER_H__

#include "Macros.h"
#include "Debug.h"

#define ZELOG(logGroup, logType, logFormat, ... ) \
	ZE::LogManager::getInstance()->PrintLog(logGroup, #logGroup, logType, logFormat, __VA_ARGS__);

#define ZEINFO(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZooidEngine: " ## textFormat ## "\r\n", __VA_ARGS__);

#define ZEWARNING(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZooidEngine: Warning: " ## textFormat ## "\r\n", __VA_ARGS__);

#define ZEERROR(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZooidEngine: Error: " ## textFormat ## "\r\n", __VA_ARGS__);

#include <cassert>
#define ZASSERT(condition,textFormat, ...)\
	if(condition == false) \
	{ \
		ZEERROR(textFormat, __VA_ARGS__); \
		assert(condition); \
	}

#define ZCHECK(condition) assert(condition);

#endif 
