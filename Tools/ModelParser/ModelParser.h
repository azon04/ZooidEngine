#ifndef __ZE_TOOLS_MODEL_PARSER__
#define __ZE_TOOLS_MODEL_PARSER__

#include <string>
#include <vector>
#include <map>
#include <fstream>

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>

#define SCALE_MASK (1 << 2)
#define QUAT_MASK (1 << 1)
#define TRANSLATION_MASK (1 << 0)

namespace ZETools
{
	struct Vertex
	{
		float Position[3];
		float Normal[3];
		float TexCoords[2];
		float Tangent[3];
		float Bitangent[3];

		bool bHasTextureCoord;
	};

	struct VertexBoneWeight
	{
		std::string BoneName;
		int BoneId;
		float BoneWeight;
	};

	enum TextureType
	{
		NONE,
		DIFFUSE_TEXTURE,
		SPECULAR_TEXTURE,
		NORMAL_TEXTURE,
		BUMP_TEXTURE,
		MASK_TEXTURE
	};

	struct Texture
	{
		std::string path;
		TextureType type;
	};

	struct Material
	{
		std::string name;
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
		bool hasBones = false;
		bool hasNormal = false;
		bool hasTangent = false;
		std::map<int, std::vector<VertexBoneWeight>> vertexBoneWeightMap;
		std::vector<unsigned int> indices;
		Material material;
	};

	struct Bone
	{
		std::string name;
		std::string parentName;
		float tranform[4][4];
	};

	struct AnimationKey
	{
		double time;

		short sqtMask = 0;
		float scale[3];
		float quat[4];
		float trans[3];
	};

	struct AnimationNode
	{
		std::string nodeName;
		bool bIsBone;
		unsigned int boneIndex;
		std::vector<AnimationKey> keys;
	};

	struct Animation
	{
		std::string name;
		double duration;
		double tickPerSecond;
		bool hasSkeleton;
		std::vector<AnimationNode> nodes;
	};

	struct ModelParserSettings
	{
		struct Animation 
		{
			short sqtMask = TRANSLATION_MASK | SCALE_MASK | QUAT_MASK;
			bool bRecalculateQuatRuntime = false;
			bool bRemoveNonBone = true;
			bool bCreateAdditive = false;
		} animation;

		struct MeshSettings
		{
			bool bSaveTangentSpace = true;
			bool bSaveBitangent = false;
			float meshScale = 1.0f;
		} mesh;

		struct SceneSettings
		{
			float sceneScale = 1.0f;
		} scene;

		bool bMakeScene = false;
		bool bParseMesh = true;
		bool bParseAnimation = true;
		bool bParseSkeleton = true;

		std::string skeletonPath;
	};

	class ModelParser
	{
	public:

		~ModelParser();

		bool loadFile(std::string filePath);
		void save(std::string outputDir, std::string packageName);
		
		void setSettings(const ModelParserSettings& _settings)
		{
			m_settings = _settings;
		}
		
		void setModelReference(ModelParser* modelReference);

	protected:

		void saveAnimationKey(AnimationKey& animKey, std::ofstream& stream);
		void saveBone(aiNode* node, std::ofstream& outStream, int tabNumber = 0);

		void processInitialNodes(aiNode* node);
		void processNode(aiNode* node, const aiScene* scene);
		void processMesh(aiMesh* mesh, const aiScene* scene);
		void processBone(aiBone* bone, const aiScene* scene);
		void processBones(aiNode* node);
		void processAnimation(aiAnimation* anim);

		aiNode* findNodeByName(std::string name);

		std::vector<Texture> processTextures(aiMaterial* mat, aiTextureType type);

		TextureType getTextureType(aiTextureType type);
		std::string getFullPath(std::string outputDir, std::string path);

	protected:
		std::vector<aiNode*> m_aiNodes;

		std::string m_assetDir;
		std::vector<Mesh> m_meshes;
		std::string m_fileName;
		std::map<aiNode*, bool> m_boneMarkMap;
		std::map<aiNode*, aiMatrix4x4> m_boneTransformMap;
		std::vector<Bone> m_bones;
		std::map<std::string, unsigned int> m_boneToIndexMap;
		std::vector<Animation> m_animations;
		Assimp::Importer m_importer;

		ModelParserSettings m_settings;

		ModelParser* m_modelReference = nullptr;
	};
}
#endif
