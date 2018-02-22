#ifndef __ZE_TOOLS_MODEL_PARSER__
#define __ZE_TOOLS_MODEL_PARSER__

#include <string>
#include <vector>

#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace ZETools
{
	struct Vertex
	{
		float Position[3];
		float Normal[3];
		float TexCoords[3];
	};

	enum TextureType
	{
		NONE,
		DIFFUSE_TEXTURE,
		SPECULAR_TEXTURE,
		NORMAL_TEXTURE
	};

	struct Texture
	{
		std::string path;
		TextureType type;
	};

	struct Mesh
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		std::vector<Texture> textures;
	};

	class ModelParser
	{
	public:
		bool loadFile(std::string filePath);
		void save(std::string outputDir, std::string packageName);
		
	protected:

		void processNode(aiNode* node, const aiScene* scene);
		void processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> processMaterial(aiMaterial* mat, aiTextureType type);

		TextureType getTextureType(aiTextureType type);
	protected:
		std::string m_directory;
		std::vector<Mesh> m_meshes;
		std::string m_fileName;
	};
}
#endif
