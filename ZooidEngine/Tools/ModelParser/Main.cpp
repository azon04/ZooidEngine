#include <iostream>
#include "ModelParser.h"

#include <string>
#include <cstring>

using namespace std;

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Specify the file need to export. --help for help" << endl;
	}

	std::string filePath = argv[1];
	if (filePath.compare("--help") == 0)
	{
		// #TODO write help here
		return 0;
	}

	ZETools::ModelParser parser;
	std::string package = "default";
	std::string outputDir = "";

	int index = 2;
	while (index < argc)
	{
		index++;
	}

	parser.loadFile(filePath);
	parser.save(outputDir, package);

	return 0;
};