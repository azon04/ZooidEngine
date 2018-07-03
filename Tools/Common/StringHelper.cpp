#include "StringHelper.h"


std::string replaceStr(std::string str, const char* replace, const char* newString)
{
	size_t found = str.find_first_of(replace);
	int length = strlen(replace);

	while (found != std::string::npos)
	{
		str.replace(str.begin() + found, str.begin() + found + length, newString);
		found = str.find_first_of(replace);
	}

	return str;
}
std::string ZETools::StringHelper::CleanString(const std::string& text)
{
	std::string result = replaceStr(text, " ", "");
	return result;
}
