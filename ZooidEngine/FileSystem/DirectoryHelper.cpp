#include "DirectoryHelper.h"

#include <stdio.h>

namespace ZE
{
	ZE::String GetPackageAssetPath(const char* packageName, const char* assetType, const char* assetName)
	{
		String path(1024);
		sprintf_s(path.c_str(), 1024, "%s/%s/%s/%s", PACKAGE_ROOT_PATH, packageName, assetType, assetName);
		return path;
	}

	ZE::String GetEngineAssetPath(const char* assetType, const char* assetName)
	{
		String path(1024);
		sprintf_s(path.c_str(), 1024, "%s/%s/%s", "ZooidEngine", assetType, assetName);
		return path;
	}

	ZE::String GetResourcePath(const char* resourcePath)
	{
		String path(1024);
		sprintf_s(path.c_str(), 1024, "%s/%s", PACKAGE_ROOT_PATH, resourcePath);
		return path;
	}

	ZE::String GetLocationPath(const char* filePath)
	{
		String path(1024);

		int index = 0;
		int lastSep = -1;
		char c = filePath[index];
		while (c != '\0')
		{
			path[index] = c;

			if (c == '/' || c == '\\')
			{
				lastSep = index;
			}

			c = filePath[++index];
		}

		if (lastSep >= 0 && path.length() - 1 > lastSep)
		{
			path[lastSep + 1] = '\0';
		}

		return path;
		
	}

}