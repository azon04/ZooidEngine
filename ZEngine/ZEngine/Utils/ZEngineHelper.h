#ifndef __ZENGINE_HELPER_H__ 
#define __ZENGINE_HELPER_H__

#include <cassert>
#define ZASSERT(condition,message)\
	assert(condition && message)

#include "Debug.h"
#define ZEINFO(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZEINFO: " ## textFormat ## "\r\n", __VA_ARGS__);

#define ZEWARNING(textFormat, ...) \
	ZE::Debug::PrintDebugString("ZEWARNING: " ## textFormat ## "\r\n", __VA_ARGS__);

#endif 
