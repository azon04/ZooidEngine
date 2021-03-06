#ifndef __ZE_DIR_HELPER_H__
#define __ZE_DIR_HELPER_H__

#include "Utils/String.h"

#define PACKAGE_ROOT_PATH "Resources"
#define GET_PACKAGE_PATH(path) PACKAGE_ROOT_PATH path

namespace ZE
{
	// Get package asset path, based on package name, asset type and asset name (with extension)
	String GetPackageAssetPath(const char* packageName, const char* assetType, const char* assetName);

	// Get Engine asset path, based on asset type and asset name (with file extension)
	String GetEngineAssetPath(const char* assetType, const char* assetName);

	// Get absolute package asset path using relative resource path
	String GetResourcePath(const char* resourcePath);

	// Get Location Path from full filepath
	String GetLocationPath(const char* filePath);
}

#endif
