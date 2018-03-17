#include "LogManager.h"
#include "Utils/Debug.h"

#include <stdarg.h>

#define LOG_TYPED(logGroupName, logType, text) \
	ZE::Debug::PrintDebugString("ZooidEngine: [%s] %s: %s \r\n", logGroupName, #logType, text );

#define LOG_FILE(logWriter, logGroupName, logType, text) \
	logWriter.writeFormatText( "ZooidEngine: [%s] %s: %s \r\n", logGroupName, #logType, text );

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

	void LogManager::PrintLog(UInt32 logGroup, const char* logGroupName, LogType logType, const char* logFormat, ...)
	{
		if ((1 << logGroup) & m_logMask)
		{
			char szBuff[1024];
			va_list arg;
			va_start(arg, logFormat);
			_vsnprintf(szBuff, sizeof(szBuff), logFormat, arg);
			va_end(arg);

			switch (logType)
			{
			case ZE::Log:
				LOG_TYPED(logGroupName, Log, szBuff);
				LOG_FILE(m_logWriter, logGroupName, Log, szBuff);
				break;
			case ZE::Warning:
				LOG_TYPED(logGroupName, Warning, szBuff);
				LOG_FILE(m_logWriter, logGroupName, Log, szBuff);
				break;
			case ZE::Error:
				LOG_TYPED(logGroupName, Error, szBuff);
				LOG_FILE(m_logWriter, logGroupName, Log, szBuff);
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