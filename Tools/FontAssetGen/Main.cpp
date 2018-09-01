#include <iostream>

#include "FontGen.h"
#include "../Common/Dir.h"

#include <string>

#define RENDER_TEX 0
#define RENDER_SDF 1
#define RENDER_PSDF 2
#define RENDER_MSDF 3

#define COMPARE_ARG1(arg, val) (strcmp(arg, val) == 0)
#define COMPARE_ARG2(arg, val, altval) (strcmp(arg, val) == 0 || strcmp(argv[index], altval) == 0)

using namespace std;

void showHelp()
{
	cout << "Zooid Engine Font Texture Generator v0.0.1" << endl;
	cout << "This tool is using Freetype library and MSDFGen https://github.com/Chlumsky/msdfgen" << endl;
	cout << "Currently support TTF font file." << endl;
	cout << "FontGent filePath [-o outputDir | --out outputDir] [-p packageName | --package packageName] [-pxh pixelHeight | --pixelHeight pixelHeight] [-rm renderMethod | --renderMethod renderMethod] [-d | --debug]" << endl;
	cout << "-o | -out path \t Output Directory" << endl;
	cout << "-p | --package path \t package name, package that assets will be save to. It will outDir/packageName" << endl;
	cout << "-pxh | --pixelHeight value \t set pixel height for font. " << endl;
	cout << "-rm | --renderMethod {TEX|SDF|PSDF|MSDF} \t Render method that will be used for the font in the engine. And it will affect the result texture too" << endl;
	cout << "-d | --debug \t Debug output will result an image for each font glyph in the font file" << endl;
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		cout << "Specify the file need to export." << endl;
		showHelp();
		return 0;
	}

	std::string filePath = argv[1];
	std::string outDir = ".";
	std::string packageName = "Default";
	int pixelHeight = 32;
	int renderMethod = RENDER_TEX;
	bool bDebug = false;

	if (filePath.compare("--help") == 0)
	{
		showHelp();
		return 0;
	}

	int index = 2;
	while (index < argc)
	{
		char* arg = argv[index++];
		if (COMPARE_ARG2(arg, "-o", "--output"))
		{
			outDir = argv[index++];
		}
		else if (COMPARE_ARG2(arg, "-p", "--package"))
		{
			packageName = argv[index++];
		}
		else if (COMPARE_ARG2(arg, "-pxh", "--pixelHeight"))
		{
			pixelHeight = atoi(argv[index++]);
		}
		else if (COMPARE_ARG2(arg, "-rm", "--renderMethod"))
		{
			const char* renderArg = argv[index++];
			if (COMPARE_ARG1(renderArg, "TEX"))
			{
				renderMethod = RENDER_TEX;
			}
			else if (COMPARE_ARG1(renderArg, "SDF"))
			{
				renderMethod = RENDER_SDF;
			}
			else if (COMPARE_ARG1(renderArg, "PSDF"))
			{
				renderMethod = RENDER_PSDF;
			}
			else if (COMPARE_ARG1(renderArg, "MSDF"))
			{
				renderMethod = RENDER_MSDF;
			}
		}
		else if (COMPARE_ARG2(arg, "-d", "--debug"))
		{
			bDebug = true;
		}
	}

	std::string fullOutputPath = ZETools::Dir::CombinePath(outDir, packageName);
	fullOutputPath = ZETools::Dir::CombinePath(fullOutputPath, "Font");
	ZETools::Dir::CreateDirectory(fullOutputPath);

	switch (renderMethod)
	{
	case RENDER_TEX:
		generateFontMap(filePath.c_str(), fullOutputPath.c_str(), pixelHeight, bDebug);
		break;
	case RENDER_SDF:
		generateFontSDF(filePath.c_str(), fullOutputPath.c_str(), pixelHeight, bDebug);
		break;
	case RENDER_PSDF:
		generateFontPSDF(filePath.c_str(), fullOutputPath.c_str(), pixelHeight, bDebug);
		break;
	case RENDER_MSDF:
		generateFontMSDF(filePath.c_str(), fullOutputPath.c_str(), pixelHeight, bDebug);
		break;
	default:
		break;
	}
	
	return 0;
}