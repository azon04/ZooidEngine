#include "LogManager.h"
#include "Utils/Debug.h"

#include <stdarg.h>

// Write to development output log
#define LOG_DEV_TYPED(logGroupName, logType, text) \
	ZE::Debug::PrintDebugString("ZooidEngine: [%s] %s: %s \r\n", logGroupName, #logType, text );

// Write to Console (if any)
#define LOG_CONSOLE_TYPE(logGroupName, logType, text) \
	printf("ZooidEngine: [%s] %s: %s \r\n", logGroupName, #logType, text);

#define LOG_FILE(logWriter, logGroupName, logType, text) \
	logWriter.writeFormatText( "ZooidEngine: [%s] %s: %s \r\n", logGroupName, #logType, text );

#define COMPARE_LOGGROUP_RETURN(logGroup1, logGroup2) \
	if(logGroup1 == logGroup2) { return #logGroup1; }

namespace ZE
{

	LogManager::LogManager()
		: m_logMask(0xFFFFFFFF)
	{
		DisableLogGroup(LOG_MEMORY);
		// #TODO Need to improve Logging file (too intrusive)
		m_logWriter = FileWriter("Log.txt");
	}

	LogManager::~LogManager()
	{
		m_logWriter.close();
	}

	ZE::LogManager* LogManager::getInstance()
	{
		static LogManager logManager;

		return &logManager;
	}

	const char* getLogGroupName(UInt32 logGroup)
	{
		COMPARE_LOGGROUP_RETURN(LOG_ENGINE, logGroup);
		COMPARE_LOGGROUP_RETURN(LOG_GAME, logGroup);
		COMPARE_LOGGROUP_RETURN(LOG_RENDERING, logGroup);
		COMPARE_LOGGROUP_RETURN(LOG_MEMORY, logGroup);
		COMPARE_LOGGROUP_RETURN(LOG_PHYSICS, logGroup);
		return "\0";
	}

	void LogManager::PrintLog(UInt32 logGroup, const char* logGroupNameParam, LogType logType, const char* logFormat, ...)
	{
		if ((1 << logGroup) & m_logMask)
		{
			const char* logGroupName = getLogGroupName(logGroup);
			if (logGroupName[0] == '\0')
			{
				logGroupName = logGroupNameParam;
			}

			char szBuff[1024];
			va_list arg;
			va_start(arg, logFormat);
			_vsnprintf(szBuff, sizeof(szBuff), logFormat, arg);
			va_end(arg);

			switch (logType)
			{
			case ZE::Log:
				LOG_DEV_TYPED(logGroupName, Log, szBuff);
				LOG_CONSOLE_TYPE(logGroupName, Log, szBuff);
				LOG_FILE(m_logWriter, logGroupName, Log, szBuff);
				break;
			case ZE::Warning:
				LOG_DEV_TYPED(logGroupName, Warning, szBuff);
				LOG_CONSOLE_TYPE(logGroupName, Warning, szBuff);
				LOG_FILE(m_logWriter, logGroupName, Warning, szBuff);
				break;
			case ZE::Error:
				LOG_DEV_TYPED(logGroupName, Error, szBuff);
				LOG_CONSOLE_TYPE(logGroupName, Error, szBuff);
				LOG_FILE(m_logWriter, logGroupName, Error, szBuff);
				break;
			default:
				break;
			}
		}
	}

	void LogManager::EnableLogGroup(UInt32 logGroup)
	{
		m_logMask = m_logMask | (1 << logGroup);
	}

	void LogManager::DisableLogGroup(UInt32 logGroup)
	{
		m_logMask = m_logMask & ~(1 << logGroup);
	}

}