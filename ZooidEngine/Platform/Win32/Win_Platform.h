#ifndef __ZE_WIN_PLATFORM_H__
#define __ZE_WIN_PLATFORM_H__

#include <windows.h>

#define ZE_MAIN() INT WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nShowCmd)

#define ZE_INITPLATFORM() ZE::PlatformArgs platArgs; \
	platArgs.rawCommandLine = lpCmdLine; \
	ZE::Platform::Init(platArgs);

#define ZE_POSTINITPLATFORM() ZE::Platform::PostInit();

#define ZE_DESTROYPLATFORM() ZE::Platform::Destroy(); \
	return ZE::Platform::GetExitErrorCode();

#define ZE_PLATFORMLOOP() while(!ZE::Platform::IsRequestedExit())

namespace ZE
{
	struct PlatformArgs
	{
		LPSTR rawCommandLine;
		int paramCount;
		char* paramArgs[64];
	};
}
#endif
