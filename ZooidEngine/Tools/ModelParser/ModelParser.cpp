#include "ModelParser.h"

#include <assimp/Importer.hpp>
#include <iostream>
#include <fstream>
#include <stdio.h>

#include "../Common/Dir.h"

namespace ZETools
{

	bool ModelParser::loadFile(std::string filePath)
	{
		Assimp::Importer import;
		const aiScene* scene = import.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
		
		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			return false;
		}

		m_directory = filePath.substr(0, filePath.find_last_of('/'));
		m_fileName = filePath.substr(filePath.find_last_of('/'));

		processNode(scene->mRootNode, scene);
	}

	void ModelParser::save(std::string outputDir, std::string packageName)
	{
		// #TODO create folder structure if not present
		std::string packagePath;
		if (outputDir.length() > 0)
		{
			packagePath += outputDir + Dir::separator();
		}
		packagePath += packageName;

		std::string vertexBufferPath = Dir::CombinePath( packagePath, "VertexBuffer");
		std::string texturePath = Dir::CombinePath(packagePath, "Texture");
		std::string materialPath = Dir::CombinePath(packagePath, "Material");
		
		Dir::CreateDirectory(vertexBufferPath);
		Dir::CreateDirectory(texturePath);
		Dir::CreateDirectory(materialPath);

		for (int i = 0; i < m_meshes.size(); i++)
		{
			Mesh& mesh = m_meshes[i];

			// #TODO export vbuff and indices buffer to file
			std::string meshFileName = mesh.name.length() > 0 ? mesh.name : m_fileName + "_" + std::to_string(i);
			{
				std::ofstream stream;
				stream.open(Dir::CombinePath(vertexBufferPath, meshFileName + ".vbuff").c_str(), std::ofstream::out);
				if (stream.is_open())
				{
					stream << "BUFFER_LAYOUT_V3_N3_TC2\n";
					stream << mesh.vertices.size() << "\n";
					for (int vi = 0; vi < mesh.vertices.size(); vi++)
					{
						stream << mesh.vertices[vi].Position[0] << " " << mesh.vertices[vi].Position[2] << " " << mesh.vertices[vi].Position[3] << "\t";
						stream << mesh.vertices[vi].Normal[0] << " " << mesh.vertices[vi].Normal[1] << " " << mesh.vertices[vi].Normal[3] << "\t";
						stream << mesh.vertices[vi].TexCoords[0] << " " << mesh.vertices[vi].TexCoords[1] << "\n";
					}

					if (mesh.indices.size() > 0)
					{
						stream << "INDICE_BUFFER" << std::endl;
						stream << mesh.indices.size() << std::endl;
					}
					else
					{
						stream << "INDICE_BUFFER_NONE" << std::endl;
					}

					for (int j = 0; j < mesh.indices.size(); j++)
					{
						stream << mesh.indices[j] << std::endl;
					}

					stream.close();
				}

				// #TODO export material
				// #TODO copying texture files if necessary
				{
					std::ofstream stream;
					stream.open(Dir::CombinePath(materialPath, meshFileName + ".matz"));
					if (stream.is_open())
					{
						for (int j = 0; j < mesh.textures.size(); j++)
						{
							Texture& texture = mesh.textures[j];
							switch (texture.type)
							{
							case DIFFUSE_TEXTURE:
								stream << "diffuse ";
								break;
							case NORMAL_TEXTURE:
								stream << "normals ";
								break;
							case SPECULAR_TEXTURE:
								stream << "specular ";
								break;
							default:
								break;
							}
							
							stream << Dir::CombinePath(texturePath, texture.path) << std::endl;

							// Copy Files
							std::ifstream source(Dir::CombinePath(m_directory, texture.path), std::ofstream::binary);
							if (source.is_open())
							{
								std::ofstream dest(Dir::CombinePath(texturePath, texture.path), std::ofstream::binary);

								dest << source.rdbuf();

								source.close();
								dest.close();
							}
						}

						stream.close();
					}
				}
				
				// #TODO create meshz file

			}



		}
	}

	void ModelParser::processNode(aiNode* node, const aiScene* scene)
	{
		for (int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}

		for (int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void ModelParser::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		Mesh outMesh;

		outMesh.name = mesh->mName.C_Str();

		for (int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex pos = 
				{ mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z,
					mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z,
					0.0f, 0.0f
				};

			if (mesh->mTextureCoords[0])
			{
				// #TODO need to support multiple texture coordinates
				pos.TexCoords[0] = mesh->mTextureCoords[0][i].x;
				pos.TexCoords[1] = mesh->mTextureCoords[0][i].y;
			}

			outMesh.vertices.push_back(pos);
		}

		// Indices
		for (int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (int j = 0; j < face.mNumIndices; j++)
			{
				outMesh.indices.push_back(face.mIndices[j]);
			}
		}

		// Material
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			for (aiTextureType type = aiTextureType_NONE; type < aiTextureType_UNKNOWN; )
			{
				std::vector<Texture> res = processMaterial(material, type);
				outMesh.textures.insert(outMesh.textures.end(), res.begin(), res.end());
				type = (aiTextureType)(static_cast<unsigned int>(type) + 1);
			}
		}

		m_meshes.push_back(outMesh);
	}

	std::vector<ZETools::Texture> ModelParser::processMaterial(aiMaterial* mat, aiTextureType type)
	{
		std::vector<Texture> res;
		for (int i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			Texture texture;
			texture.path = str.C_Str();
			texture.type = getTextureType(type);
			res.push_back(texture);
		}
		return res;
	}

	ZETools::TextureType ModelParser::getTextureType(aiTextureType type)
	{
		switch (type)
		{
		case aiTextureType_NONE:
			break;
		case aiTextureType_DIFFUSE:
			return DIFFUSE_TEXTURE;
			break;
		case aiTextureType_SPECULAR:
			return SPECULAR_TEXTURE;
			break;
		case aiTextureType_AMBIENT:
			break;
		case aiTextureType_EMISSIVE:
			break;
		case aiTextureType_HEIGHT:
			return NORMAL_TEXTURE; // TODO will support later
			break;
		case aiTextureType_NORMALS:
			return NORMAL_TEXTURE;
			break;
		case aiTextureType_SHININESS:
			break;
		case aiTextureType_OPACITY:
			break;
		case aiTextureType_DISPLACEMENT:
			break;
		case aiTextureType_LIGHTMAP:
			break;
		case aiTextureType_REFLECTION:
			break;
		case aiTextureType_UNKNOWN:
			break;
		case _aiTextureType_Force32Bit:
			break;
		default:
			break;
		}

		return NONE;
	}

}