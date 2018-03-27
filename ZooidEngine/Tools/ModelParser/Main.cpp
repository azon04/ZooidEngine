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
		return 0;
	}

	std::string filePath = argv[1];
	if (filePath.compare("--help") == 0)
	{
		cout << "Zooid Engine Mesh Parser Tools v0.0.1" << endl;
		cout << "This tool is using Assimp check for more at assimp.org" << endl;
		cout << "MeshParser filepath [-o outputDir | --out outputDir] [-p packageName | --package packageName]" << endl;
		cout << "-o | --out path \t Output directory" << endl;
		cout << "-p | --package path \t package name, package that assets will be saved to. It will outDir/packageName" << endl;

		return 0;
	}

	ZETools::ModelParser parser;
	std::string package = "default";
	std::string outputDir = "";

	int index = 2;
	while (index < argc)
	{
		if (strcmp(argv[index], "--out") == 0 || strcmp(argv[index], "-o") == 0)
		{
			outputDir = argv[++index];
		}
		else if (strcmp(argv[index], "--package") == 0 || strcmp(argv[index], "-p") == 0)
		{
			package = argv[++index];
		}
		index++;
	}


	parser.loadFile(filePath);
	parser.save(outputDir, package);

	return 0;
};