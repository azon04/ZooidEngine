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
		float TexCoords[2];
	};

	enum TextureType
	{
		NONE,
		DIFFUSE_TEXTURE,
		SPECULAR_TEXTURE,
		NORMAL_TEXTURE,
		BUMP_TEXTURE
	};

	struct Texture
	{
		std::string path;
		TextureType type;
	};

	struct Material
	{
		float Ka[3];
		float Kd[3];
		float Ks[3];
		float shininess;
		std::vector<Texture> textures;
	};

	struct Mesh
	{
		std::string name;
		std::vector<Vertex> vertices;
		std::vector<unsigned int> indices;
		Material material;
	};

	class ModelParser
	{
	public:
		bool loadFile(std::string filePath);
		void save(std::string outputDir, std::string packageName);
		
	protected:

		void processNode(aiNode* node, const aiScene* scene);
		void processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> processTextures(aiMaterial* mat, aiTextureType type);

		TextureType getTextureType(aiTextureType type);
		std::string getFullPath(std::string outputDir, std::string path);

	protected:
		std::string m_assetDir;
		std::vector<Mesh> m_meshes;
		std::string m_fileName;
	};
}
#endif
