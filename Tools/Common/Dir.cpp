#include "Dir.h"

#include <io.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#include <direct.h>
#endif

bool ZETools::Dir::CreateDirectory(std::string path)
{
	if (_access(path.c_str(), 0) == 0)
	{
		return true;
	}

	if (path.find_last_of("\\") == std::string::npos && path.find_last_of("/") == std::string::npos)
	{
		return _mkdir(path.c_str()) == 0;
	}
	else
	{
		std::string::size_type lastDelim = path.find_last_of("\\");
		if (lastDelim == std::string::npos)
		{
			lastDelim = path.find_last_of("/");
		}

		bool res = CreateDirectory(path.substr(0, lastDelim));

		return res && _mkdir(path.c_str()) == 0;
	}

	return false;
}

std::string ZETools::Dir::CombinePath(std::string path1, std::string path2)
{
	return path1 + Dir::separator() + path2;
}

bool ZETools::Dir::IsExist(std::string path)
{
	if (FILE *file = fopen(path.c_str(), "r"))
	{
		fclose(file);
		return true;
	}

	return false;
}

