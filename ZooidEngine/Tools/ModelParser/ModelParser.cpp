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

		m_assetDir = filePath.substr(0, filePath.find_last_of(Dir::separator()));
		m_fileName = filePath.substr(filePath.find_last_of(Dir::separator()));

		processNode(scene->mRootNode, scene);

		return true;
	}

	void ModelParser::save(std::string outputDir, std::string packageName)
	{
		// Create folder structure if not present
		std::string packagePath = packageName;

		std::string vertexBufferPath = Dir::CombinePath( packagePath, "VertexBuffer");
		std::string texturePath = Dir::CombinePath(packagePath, "Texture");
		std::string materialPath = Dir::CombinePath(packagePath, "Material");
		std::string meshPath = Dir::CombinePath(packagePath, "Mesh");

		Dir::CreateDirectory(getFullPath(outputDir, vertexBufferPath));
		Dir::CreateDirectory(getFullPath(outputDir, texturePath));
		Dir::CreateDirectory(getFullPath(outputDir, materialPath));
		Dir::CreateDirectory(getFullPath(outputDir, meshPath));

		for (unsigned int i = 0; i < m_meshes.size(); i++)
		{
			Mesh& mesh = m_meshes[i];

			std::string meshFileName = mesh.name.length() > 0 ? mesh.name : m_fileName + "_" + std::to_string(i);
			std::string vertexFilePath = Dir::CombinePath(vertexBufferPath, meshFileName + ".vbuff");
			std::string matFilePath = Dir::CombinePath(materialPath, meshFileName + ".matz");
			std::string meshFilePath = Dir::CombinePath(meshPath, meshFileName + ".meshz");

			// Export vbuff and indices buffer to file
			{
				std::ofstream stream;
				stream.open(getFullPath(outputDir, vertexFilePath), std::ofstream::out);
				if (stream.is_open())
				{
					stream << "BUFFER_LAYOUT_V3_N3_TC2\n";
					stream << mesh.vertices.size() << "\n";
					for (unsigned int vi = 0; vi < mesh.vertices.size(); vi++)
					{
						stream << mesh.vertices[vi].Position[0] << " " << mesh.vertices[vi].Position[1] << " " << mesh.vertices[vi].Position[2] << "\t";
						stream << mesh.vertices[vi].Normal[0] << " " << mesh.vertices[vi].Normal[1] << " " << mesh.vertices[vi].Normal[2] << "\t";
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

					for (unsigned int j = 0; j < mesh.indices.size(); j++)
					{
						stream << mesh.indices[j] << std::endl;
					}

					stream.close();
				}
				else
				{
					std::cout << "ERROR: Can't create \"" << getFullPath(outputDir, vertexFilePath) << "\"" << std::endl;
				}
			}

			// Export material
			// Copying texture files if necessary
			{
				std::ofstream stream;
				stream.open(getFullPath(outputDir, matFilePath));
				if (stream.is_open())
				{
					Material& mat = m_meshes[i].material;
					stream << "Ka " << mat.Ka[0] << " " << mat.Ka[1] << " " << mat.Ka[2] << std::endl;
					stream << "Kd " << mat.Kd[0] << " " << mat.Kd[1] << " " << mat.Kd[2] << std::endl;
					stream << "Ks " << mat.Ks[0] << " " << mat.Ks[1] << " " << mat.Ks[2] << std::endl;
					stream << "shininess " << mat.shininess << std::endl;

					for (unsigned int j = 0; j < mat.textures.size(); j++)
					{
						Texture& texture = mat.textures[j];
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
						case BUMP_TEXTURE:
							stream << "bump ";
						default:
							stream << "map ";
							break;
						}
							
						stream << Dir::CombinePath(texturePath, texture.path) << std::endl;

						// Copy Files
						std::ifstream source(Dir::CombinePath(m_assetDir, texture.path), std::ofstream::binary);
						if (source.is_open())
						{
							std::ofstream dest(getFullPath(outputDir, Dir::CombinePath(texturePath, texture.path)), std::ofstream::binary);

							dest << source.rdbuf();

							source.close();
							dest.close();
						}
						else
						{
							std::cout << "WARNING: Can't copy texture \"" << Dir::CombinePath(m_assetDir, texture.path) << "\". The texture might set something else in the game." << std::endl;
						}
					}

					stream.close();
				}
				else
				{
					std::cout << "ERROR: Can't create \"" << getFullPath(outputDir, matFilePath) << "\"" << std::endl;
				}
			}
				
			// #TODO create meshz file
			{
				std::ofstream stream(getFullPath(outputDir, meshFilePath));
				if (stream.is_open())
				{
					stream << "vbuff " << vertexFilePath << std::endl;
					stream << "mat " << matFilePath << std::endl;

					stream.close();
				}
				else
				{
					std::cout << "ERROR: Can't create \"" << getFullPath(outputDir, meshFilePath) << "\"" << std::endl;
				}
			}
			
		}
	}

	void ModelParser::processNode(aiNode* node, const aiScene* scene)
	{
		for (unsigned int i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			processMesh(mesh, scene);
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	void ModelParser::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		Mesh outMesh;

		outMesh.name = mesh->mName.C_Str();

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			Vertex pos;
			pos.Position[0] = mesh->mVertices[i].x;
			pos.Position[1] = mesh->mVertices[i].y;
			pos.Position[2] = mesh->mVertices[i].z;
			pos.Normal[0] = mesh->mNormals[i].x;
			pos.Normal[1] = mesh->mNormals[i].y;
			pos.Normal[2] = mesh->mNormals[i].z;
			pos.TexCoords[0] = 0.0f;
			pos.TexCoords[1] = 0.0f;

			if (mesh->mTextureCoords[0])
			{
				// #TODO need to support multiple texture coordinates
				pos.TexCoords[0] = mesh->mTextureCoords[0][i].x;
				pos.TexCoords[1] = mesh->mTextureCoords[0][i].y;
			}

			outMesh.vertices.push_back(pos);
		}

		// Indices
		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			aiFace& face = mesh->mFaces[i];
			for (unsigned int j = 0; j < face.mNumIndices; j++)
			{
				outMesh.indices.push_back(face.mIndices[j]);
			}
		}

		// Material
		if (mesh->mMaterialIndex >= 0)
		{
			Material outMat;
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

			aiColor3D MatKa;
			aiColor3D MatKd;
			aiColor3D MatKs;
			ai_real shininess;
			material->Get(AI_MATKEY_COLOR_AMBIENT, MatKa);
			material->Get(AI_MATKEY_COLOR_DIFFUSE, MatKd);
			material->Get(AI_MATKEY_COLOR_SPECULAR, MatKs);

			material->Get(AI_MATKEY_SHININESS, shininess);

			outMat.Ka[0] = MatKa.r;
			outMat.Ka[1] = MatKa.g;
			outMat.Ka[2] = MatKa.b;

			outMat.Kd[0] = MatKd.r;
			outMat.Kd[1] = MatKd.g;
			outMat.Kd[2] = MatKd.b;

			outMat.Ks[0] = MatKs.r;
			outMat.Ks[1] = MatKs.g;
			outMat.Ks[2] = MatKs.b;

			outMat.shininess = shininess;

			for (aiTextureType type = aiTextureType_NONE; type < aiTextureType_UNKNOWN; )
			{
				std::vector<Texture> res = processTextures(material, type);
				outMat.textures.insert(outMat.textures.end(), res.begin(), res.end());
				type = (aiTextureType)(static_cast<unsigned int>(type) + 1);
			}
			outMesh.material = outMat;
		}

		m_meshes.push_back(outMesh);
	}

	std::vector<ZETools::Texture> ModelParser::processTextures(aiMaterial* mat, aiTextureType type)
	{
		std::vector<Texture> res;
		for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
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
			return BUMP_TEXTURE;
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

	std::string ModelParser::getFullPath(std::string outputDir, std::string path)
	{
		if (outputDir.length() > 0)
		{
			return Dir::CombinePath(outputDir, path);
		}

		return path;
	}

}