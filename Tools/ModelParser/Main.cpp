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
		cout << "--omitAnimChannel [S|Q|T] \t omit animation channel, S: Scale, Q: Quaternion, T: Translation. e.g omit translation and Scale would use ST " << endl;
		cout << "--recalculateAnimQuat \t this option will make Quaternion in animation data saved only 3 values (the fourth will be calculated on runtime)  " << endl;
		cout << "--meshScale scale \t Scale all the meshes inside this model by the scale factor." << endl;
		cout << "--noMesh" << endl;
		cout << "--noAnimation" << endl;
		cout << "--skeletonPath path \t path is relative path of skeleton from out directory" << endl;
		cout << "--reference path \t path is FBX or raw assets path to use as reference (for making additive animation)" << endl;
		cout << "--omitTangentSpace \t option to not save tangent space " << endl;
		cout << "--withBitangent \t option to also save bitangent of the mesh" << endl;
		cout << "--saveScene \t option to save the mesh file into scene file too (work best if you have multiple meshes in mesh file)" << endl;
		cout << "--sceneCale scale \t This option will scale the mesh in the scene definition, not scale the actual model" << endl;
		return 0;
	}

	ZETools::ModelParser parser;
	std::string package = "default";
	std::string outputDir = "";

	std::string referencePath = "";

	ZETools::ModelParserSettings settings;

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
		else if (strcmp(argv[index], "--omitAnimChannel") == 0)
		{
			char* sqtString = argv[++index];
			int sqtIndex = 0;
			while (sqtString[sqtIndex])
			{
				if (sqtString[sqtIndex] == 'S')
				{
					settings.animation.sqtMask &= ~(0xff & SCALE_MASK);
				}
				else if (sqtString[sqtIndex] == 'Q')
				{
					settings.animation.sqtMask &= ~(0xff & QUAT_MASK);
				}
				else if (sqtString[sqtIndex] == 'T')
				{
					settings.animation.sqtMask &= ~(0xff & TRANSLATION_MASK);
				}
				sqtIndex++;
			}
		} 
		else if (strcmp(argv[index], "--recalculateAnimQuat") == 0)
		{
			settings.animation.bRecalculateQuatRuntime = true;
		}
		else if (strcmp(argv[index], "--meshScale") == 0)
		{
			char* scaleStr = argv[++index];
			settings.mesh.meshScale = atof(scaleStr);
		}
		else if (strcmp(argv[index], "--noMesh") == 0)
		{
			settings.bParseMesh = false;
		}
		else if (strcmp(argv[index], "--noAnimation") == 0)
		{
			settings.bParseAnimation = false;
		}
		else if (strcmp(argv[index], "--skeletonPath") == 0)
		{
			settings.bParseSkeleton = false;
			settings.skeletonPath = argv[++index];
		}
		else if (strcmp(argv[index], "--reference") == 0)
		{
			referencePath = argv[++index];
			settings.animation.bCreateAdditive = true;
		}
		else if (strcmp(argv[index], "--omitTangentSpace") == 0)
		{
			settings.mesh.bSaveTangentSpace = false;
		}
		else if (strcmp(argv[index], "--withBitangent") == 0)
		{
			settings.mesh.bSaveBitangent = true;
		}
		else if (strcmp(argv[index], "--saveScene") == 0)
		{
			settings.bMakeScene = true;
		}
		else if (strcmp(argv[index], "--sceneScale") == 0)
		{
			char* scaleStr = argv[++index];
			settings.scene.sceneScale = atof(scaleStr);
		}
		
		index++;
	}

	ZETools::ModelParser referenceParser;

	if (referencePath != "")
	{
		ZETools::ModelParserSettings referenceSettings;
		referenceSettings.bParseMesh = false;
		referenceParser.setSettings(referenceSettings);
		referenceParser.loadFile(referencePath);

		parser.setModelReference(&referenceParser);
	}

	parser.setSettings(settings);
	parser.loadFile(filePath);
	parser.save(outputDir, package);

	return 0;
};