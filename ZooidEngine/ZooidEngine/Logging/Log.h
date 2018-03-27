#ifndef __ZE_LOG_H__
#define __ZE_LOG_H__

#include "LogManager.h"

#define ZELOG(logGroup, logType, logFormat, ... ) \
	ZE::LogManager::getInstance()->PrintLog(logGroup, #logGroup, logType, logFormat, __VA_ARGS__);

#endif
