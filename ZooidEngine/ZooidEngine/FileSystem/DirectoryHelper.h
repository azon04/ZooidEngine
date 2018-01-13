#ifndef __ZE_DIR_HELPER_H__
#define __ZE_DIR_HELPER_H__

#include "Utils/String.h"

#define PACKAGE_ROOT_PATH "Resources"

namespace ZE
{
	String GetPackageAssetPath(const char* packageName, const char* assetType, const char* assetName);

	String GetEngineAssetPath(const char* assetType, const char* assetName);

	String GetResourcePath(const char* resourcePath);
}

#endif
