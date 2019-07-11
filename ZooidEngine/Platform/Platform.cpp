#include "Platform.h"

namespace ZE
{

	bool Platform::IsRequestedExit()
	{
		Platform* plat = GetPlatform();
		return plat->isRequestedExit();
	}

	void Platform::RequestExit(int errorCode)
	{
		Platform* plat = GetPlatform();
		return plat->requestExit(errorCode);
	}

	void Platform::Init(PlatformArgs& args)
	{
		Platform* plat = GetPlatform();
		plat->init(args);
	}

	void Platform::Destroy()
	{
		Platform* plat = GetPlatform();
		plat->destroy();
	}

	int Platform::GetExitErrorCode()
	{
		Platform* plat = GetPlatform();
		return plat->getExitErrorCode();
	}

	bool Platform::GetArgByIndex(int index, char* buff)
	{
		Platform* plat = GetPlatform();
		return plat->getArgByIndex(index, buff);
	}

	int Platform::GetArgCount()
	{
		Platform* plat = GetPlatform();
		return plat->getArgCount();
	}

	ZE::Float32 Platform::RandUnit()
	{
		return GetPlatform()->randUnitFloat();
	}

}