#include "DirectoryHelper.h"

#include <stdio.h>

namespace ZE
{
	ZE::String GetPackageAssetPath(const char* packageName, const char* assetType, const char* assetName)
	{
		String path(1024);
		sprintf(path.c_str(), "%s/%s/%s/%s", PACKAGE_ROOT_PATH, packageName, assetType, assetName);
		return path;
	}

	ZE::String GetEngineAssetPath(const char* assetType, const char* assetName)
	{
		String path(1024);
		sprintf(path.c_str(), "%s/%s/%s", "ZooidEngine", assetType, assetName);
		return path;
	}

}