#ifndef __ZE_LOG_MANAGER_H__
#define __ZE_LOG_MANAGER_H__

#include "FileSystem/FileWriter.h"

namespace ZE
{
	enum LogGroup : UInt32
	{
		LOG_ENGINE = 0,
		LOG_GAME = 1,
		LOG_RENDERING = 2,
		LOG_MEMORY = 3,
		LOG_PHYSICS = 4
	};

	enum LogType : UInt8
	{
		Log,
		Warning,
		Error
	};

	class LogManager
	{
	public:
		LogManager();
		~LogManager();
		
		static LogManager* getInstance();

		// Print Log to console and log file
		void PrintLog(UInt32 logGroup, const char* logGroupName, LogType logType, const char* logFormat, ...);

		// Enable specific log group
		void EnableLogGroup(UInt32 logGroup);

		// Disable specific log group
		void DisableLogGroup(UInt32 logGroup);
		
	protected:
		UInt32 m_logMask;
		FileWriter m_logWriter;

	};
}
#endif
