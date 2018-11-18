#ifndef __ZE_PLATFORM_H__
#define __ZE_PLATFORM_H__

#include "Utils/Macros.h"

#if defined(ZE_PLATFORM_WINDOWS)
#include "Win32/Win_Platform.h"
#else
#error No Platform Implementation for this platform
#endif

namespace ZE
{
	class Platform
	{

	protected:
		virtual void init(PlatformArgs&) = 0;
		virtual void requestExit(int errorCode) = 0;
		virtual bool isRequestedExit() = 0;
		virtual void destroy() = 0;
		virtual int getExitErrorCode() = 0;
		virtual PlatformArgs* getArgs() = 0;
		virtual bool getArgByIndex(int index, char* buff) = 0;
		virtual int getArgCount() = 0;

		// #TODO implement this on specific platform
		static Platform* GetPlatform();
	
	public:

		static bool IsRequestedExit();
		static void RequestExit(int errorCode);
		static void Init(PlatformArgs& args);
		static void Destroy();
		static int GetExitErrorCode();
		static bool GetArgByIndex(int index, char* buff);
		static int GetArgCount();
	};
}
#endif