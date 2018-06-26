#include "ModelParser.h"

#include <iostream>
#include <stdio.h>

#include "../Common/Dir.h"
#include "../Common/StringHelper.h"

#define ANIMATION_SAVE_TIME_FIRST 1

namespace ZETools
{

	ModelParser::~ModelParser()
	{
		m_importer.FreeScene();
	}

	bool ModelParser::loadFile(std::string filePath)
	{
		std::cout << "Load file " << filePath << "..." << std::endl;

		const aiScene* scene = m_importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_LimitBoneWeights | aiProcess_GenNormals);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			return false;
		}

		m_assetDir = filePath.substr(0, filePath.find_last_of(Dir::separator()));
		m_fileName = filePath.substr(filePath.find_last_of(Dir::separator()) + 1);
		
		// Cleaning the filename (remove the extension)
		m_fileName = m_fileName.substr(0, m_fileName.find_last_of('.'));

		std::cout << "Process Nodes... " << std::endl;
		processInitialNodes(scene->mRootNode);

		std::cout << "Process Meshes..." << std::endl;
		processNode(scene->mRootNode, scene);

		std::cout << "Checking Bones... " << std::endl;
		processBones(scene->mRootNode);
		
		std::cout << "Checking Animation... " << std::endl;
		for (unsigned int i = 0; i < scene->mNumAnimations; i++)
		{
			processAnimation(scene->mAnimations[i]);
		}

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
		std::string skelPath = Dir::CombinePath(packagePath, "Skeleton");
		std::string animPath = Dir::CombinePath(packagePath, "Animation");

		Dir::CreateDirectory(getFullPath(outputDir, vertexBufferPath));
		Dir::CreateDirectory(getFullPath(outputDir, texturePath));
		Dir::CreateDirectory(getFullPath(outputDir, materialPath));
		Dir::CreateDirectory(getFullPath(outputDir, meshPath));
		Dir::CreateDirectory(getFullPath(outputDir, skelPath));
		Dir::CreateDirectory(getFullPath(outputDir, animPath));

		std::cout << "Saving to [" << packagePath << "]... " << std::endl;

		if(!m_settings.bParseSkeleton)
		{
			if (!Dir::IsExist(Dir::CombinePath(outputDir, m_settings.skeletonPath)))
			{
				std::cout << "WARNING: CAN'T FIND SKELETON PATH: " << m_settings.skeletonPath << std::endl;
				std::cout << "This might cause problem in the engine" << std::endl;
			}
		}

		// TODO Create Skel Mesh
		std::string skeletonFilePath = Dir::CombinePath(skelPath, m_fileName + "_skel.skelz");
		if(m_settings.bParseSkeleton && m_bones.size() > 0)
		{
			std::cout << "Creating Skeleton file [" << skeletonFilePath << "]... " << std::endl;

			const aiScene* scene = m_importer.GetScene();
			
			std::ofstream stream;
			stream.open(getFullPath(outputDir, skeletonFilePath), std::ofstream::out);

			saveBone(scene->mRootNode, stream);

			stream.close();
		}

		if (m_settings.bParseMesh)
		{
			for (unsigned int i = 0; i < m_meshes.size(); i++)
			{
				Mesh& mesh = m_meshes[i];

				std::string meshFileName = mesh.name.length() > 0 ? mesh.name : m_fileName + "_" + std::to_string(i);
				std::string vertexFilePath = Dir::CombinePath(vertexBufferPath, meshFileName + ".vbuff");
				std::string matFilePath = Dir::CombinePath(materialPath, meshFileName + ".matz");
				std::string meshFilePath = Dir::CombinePath(meshPath, meshFileName + ".meshz");

				// Export vbuff and indices buffer to file
				{
					std::cout << "Creating Vertex Buffer file [" << vertexFilePath << "]... " << std::endl;
					std::ofstream stream;
					stream.open(getFullPath(outputDir, vertexFilePath), std::ofstream::out);
					if (stream.is_open())
					{
						if (mesh.hasBones)
						{
							stream << "BUFFER_LAYOUT_V3_N3_TC2_SKIN\n";
						}
						else
						{
							stream << "BUFFER_LAYOUT_V3_N3_TC2\n";
						}
						stream << mesh.vertices.size() << "\n";
						for (unsigned int vi = 0; vi < mesh.vertices.size(); vi++)
						{
							stream << mesh.vertices[vi].Position[0] << " " << mesh.vertices[vi].Position[1] << " " << mesh.vertices[vi].Position[2] << "\t";
							stream << mesh.vertices[vi].Normal[0] << " " << mesh.vertices[vi].Normal[1] << " " << mesh.vertices[vi].Normal[2] << "\t";
							stream << mesh.vertices[vi].TexCoords[0] << " " << mesh.vertices[vi].TexCoords[1] << "\t";
							if (mesh.hasBones)
							{
								std::vector<VertexBoneWeight> verticeWeights;
								if (mesh.vertexBoneWeightMap.find(vi) != mesh.vertexBoneWeightMap.end())
								{
									verticeWeights = mesh.vertexBoneWeightMap[vi];
								}

								for (int bi = 0; bi < 4; bi++)
								{
									if (bi < verticeWeights.size())
									{
										stream << m_boneToIndexMap[verticeWeights[bi].BoneName] << " ";
									}
									else
									{
										stream << 0 << " ";
									}
								}

								for (int bi = 0; bi < 4; bi++)
								{
									if (bi < verticeWeights.size())
									{
										stream << verticeWeights[bi].BoneWeight << " ";
									}
									else
									{
										stream << 0 << " ";
									}
								}
							}
							stream << "\n";
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
					std::cout << "Creating Material file [" << matFilePath << "]... " << std::endl;
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
								break;
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
					std::cout << "Creating Mesh file [" << meshFilePath << "]... " << std::endl;
					std::ofstream stream(getFullPath(outputDir, meshFilePath));
					if (stream.is_open())
					{
						stream << "vbuff " << vertexFilePath << std::endl;
						stream << "mat " << matFilePath << std::endl;
						if (mesh.hasBones)
						{
							stream << "skeleton " << (m_settings.bParseSkeleton ? skeletonFilePath : m_settings.skeletonPath) << std::endl;
						}
						stream.close();
					}
					else
					{
						std::cout << "ERROR: Can't create \"" << getFullPath(outputDir, meshFilePath) << "\"" << std::endl;
					}
				}
			}
		}

		// Save Animation
		if (m_settings.bParseAnimation)
		{
			for (unsigned int i = 0; i < m_animations.size(); i++)
			{
				Animation& anim = m_animations[i];

				std::string animationFilePath = Dir::CombinePath(animPath, m_fileName + "_" + anim.name + ".animz");

				std::cout << "Creating Animation file [" << animationFilePath << "]... " << std::endl;

				std::ofstream stream;
				stream.open(getFullPath(outputDir, animationFilePath), std::ofstream::out);

				stream << "AnimationClip " << anim.name << std::endl;
				stream << "duration " << anim.duration << std::endl;
				stream << "fps " << anim.tickPerSecond << std::endl;

				if (anim.hasSkeleton)
				{
					stream << "skeleton " << ( m_settings.bParseSkeleton ? skeletonFilePath : m_settings.skeletonPath ) << std::endl;
				}

#if ANIMATION_SAVE_TIME_FIRST
				for (double time = 0.0; time <= anim.duration; time += 1.0)
				{
					std::map <unsigned int, AnimationNode*> boneIndexToNodeMap;

					stream << "time " << time << std::endl;
					for (unsigned int nodeIndex = 0; nodeIndex < anim.nodes.size(); nodeIndex++)
					{
						AnimationNode& animNode = anim.nodes[nodeIndex];
						int index = 0;
						while (index < animNode.keys.size() && abs(animNode.keys[index].time - time) > 0.000001)
						{
							index++;
						}

						if (index < animNode.keys.size())
						{
							if (animNode.bIsBone)
							{
								boneIndexToNodeMap[animNode.boneIndex] = &animNode;
								continue;
							}
							else if(!m_settings.animation.bRemoveNonBone)
							{
								stream << "track " << animNode.nodeName << std::endl;
							}
							else
							{
								continue; // skip this frame
							}

							AnimationKey& animKey = animNode.keys[index];

							saveAnimationKey(animKey, stream);
						}
					}

					for (unsigned int i = 0; i < m_bones.size(); i++)
					{
						if (boneIndexToNodeMap.find(i) != boneIndexToNodeMap.end())
						{
							AnimationNode& animNode = *boneIndexToNodeMap[i];
							int index = 0;
							while (index < animNode.keys.size() && abs(animNode.keys[index].time - time) > 0.000001)
							{
								index++;
							}

							if (index < animNode.keys.size())
							{
								if (animNode.bIsBone)
								{
									stream << "bone " << animNode.boneIndex << std::endl;
								}
								else
								{
									stream << "track " << animNode.nodeName << std::endl;
								}

								AnimationKey& animKey = animNode.keys[index];

								saveAnimationKey(animKey, stream);
							}
						}
					}
				}
#else
				for (unsigned int nodeIndex = 0; nodeIndex < anim.nodes.size(); nodeIndex++)
				{
					AnimationNode& animNode = anim.nodes[nodeIndex];
					if (animNode.bIsBone)
					{
						stream << "bone " << m_boneToIndexMap[animNode.nodeName] << std::endl;
					}
					else
					{
						stream << "track " << animNode.nodeName << std::endl;
					}

					for (unsigned int keyIndex = 0; keyIndex < animNode.keys.size(); keyIndex++)
					{
						AnimationKey& animKey = animNode.keys[keyIndex];
						stream << "time " << animKey.time << std::endl;

						saveAnimationKey(animKey, stream);
					}
				}
#endif

				stream.close();
			}
		}
	}

	void ModelParser::saveAnimationKey(AnimationKey& animKey, std::ofstream& stream)
	{
		if ((animKey.sqtMask & TRANSLATION_MASK) && (m_settings.animation.sqtMask & TRANSLATION_MASK))
		{
			stream << "T " << animKey.trans[0] << " " << animKey.trans[1] << " " << animKey.trans[2] << std::endl;
		}

		if ((animKey.sqtMask & QUAT_MASK) && (m_settings.animation.sqtMask & QUAT_MASK))
		{
			if (!m_settings.animation.bRecalculateQuatRuntime)
			{
				stream << "Q " << animKey.quat[0] << " " << animKey.quat[1] << " " << animKey.quat[2] << " " << animKey.quat[3] << std::endl;
			}
			else
			{
				stream << "Q3 " << animKey.quat[0] << " " << animKey.quat[1] << " " << animKey.quat[2] << std::endl;
			}
		}

		if ((animKey.sqtMask & SCALE_MASK) && (m_settings.animation.sqtMask & SCALE_MASK))
		{
			stream << "S " << animKey.scale[0] << " " << animKey.scale[1] << " " << animKey.scale[2] << std::endl;
		}
	}

	void ModelParser::saveBone(aiNode* node, std::ofstream& outStream, int tabNumber)
	{
		if (m_boneMarkMap[node])
		{
			std::string tab = "";
			for (int i = 0; i < tabNumber; i++)
			{
				tab += "\t";
			}

			outStream << tab << "Joint " << node->mName.data << std::endl;
			outStream << tab << "{" << std::endl;
			// print transform
			outStream << tab  << "\t" << "InvXform ";
			aiMatrix4x4& boneTransform = m_boneTransformMap[node];
			for (int i = 0; i < 4; i++)
			{
				for (int j = 0; j < 4; j++)
				{
					outStream << boneTransform[j][i] << " ";
				}
			}
			outStream << std::endl;

			// Print Child
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				saveBone(node->mChildren[i], outStream, tabNumber+1);
			}
			outStream << tab << "}" << std::endl;
		}
		else
		{
			// Print Child
			for (unsigned int i = 0; i < node->mNumChildren; i++)
			{
				saveBone(node->mChildren[i], outStream, tabNumber);
			}
		}
	}

	void ModelParser::processInitialNodes(aiNode* node)
	{
		m_aiNodes.push_back(node);

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processInitialNodes(node->mChildren[i]);
		}

		// Mark as no bone
		m_boneMarkMap[node] = false;
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
		if (m_settings.bParseMesh)
		{
			Mesh outMesh;

			outMesh.name = mesh->mName.C_Str();
			bool hasNormal = mesh->mNormals != NULL;

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				Vertex pos;
				pos.Position[0] = mesh->mVertices[i].x;
				pos.Position[1] = mesh->mVertices[i].y;
				pos.Position[2] = mesh->mVertices[i].z;
				if (hasNormal)
				{
					pos.Normal[0] = mesh->mNormals[i].x;
					pos.Normal[1] = mesh->mNormals[i].y;
					pos.Normal[2] = mesh->mNormals[i].z;
				}
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

			outMesh.hasBones = mesh->mNumBones > 0;

			// Vertex Bones if any
			for (unsigned int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* bone = mesh->mBones[i];
				processBone(bone, scene);
				for (unsigned int j = 0; j < bone->mNumWeights; j++)
				{
					aiVertexWeight vertexWeight = bone->mWeights[j];
					VertexBoneWeight outVertexWeight;
					outVertexWeight.BoneName = bone->mName.data;
					outVertexWeight.BoneWeight = vertexWeight.mWeight;
					outMesh.vertexBoneWeightMap[vertexWeight.mVertexId].push_back(outVertexWeight);
				}
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
		else
		{
			for (unsigned int i = 0; i < mesh->mNumBones; i++)
			{
				aiBone* bone = mesh->mBones[i];
				processBone(bone, scene);
			}
		}
	}

	void ModelParser::processBone(aiBone* bone, const aiScene* scene)
	{
		std::string boneName = bone->mName.data;
		aiNode* boneNode = findNodeByName(boneName);

		if (!m_boneMarkMap[boneNode])
		{
			m_boneMarkMap[boneNode] = true;
			m_boneTransformMap[boneNode] = bone->mOffsetMatrix;
		}
	}

	void ModelParser::processBones(aiNode* node)
	{
		if (m_boneMarkMap[node])
		{
			Bone outBone;
			outBone.name = node->mName.data;
			outBone.parentName = m_boneMarkMap[node->mParent] ? node->mParent->mName.data : "";
			for (unsigned int i = 0; i < 4; i++)
			{
				for (unsigned int j = 0; j < 4; j++)
				{
					aiMatrix4x4& boneTransform = m_boneTransformMap[node];
					outBone.tranform[i][j] = boneTransform[i][j];
				}
			}

			m_bones.push_back(outBone);
			m_boneToIndexMap[outBone.name] = m_bones.size() - 1;
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			processBones(node->mChildren[i]);
		}
	}

	void ModelParser::processAnimation(aiAnimation* anim)
	{
		m_animations.push_back(Animation());
		Animation& outAnim = m_animations[m_animations.size() - 1];
		outAnim.name = StringHelper::CleanString(anim->mName.data);
		outAnim.duration = anim->mDuration;
		outAnim.tickPerSecond = anim->mTicksPerSecond;

		unsigned int cAnimIndex = m_animations.size() - 1;

		for (unsigned int i = 0; i < anim->mNumChannels; i++)
		{
			aiNodeAnim* nodeAnim = anim->mChannels[i];
			outAnim.nodes.push_back(AnimationNode());
			AnimationNode& outNode = outAnim.nodes[outAnim.nodes.size() - 1];
			outNode.nodeName = nodeAnim->mNodeName.data;
			outNode.bIsBone = m_boneToIndexMap.find(outNode.nodeName) != m_boneToIndexMap.end();
			if (outNode.bIsBone)
			{
				outNode.boneIndex = m_boneToIndexMap[outNode.nodeName];
				outAnim.hasSkeleton = true;
			}

			int posIndex = 0;
			int rotIndex = 0;
			int scaleIndex = 0;
			
			while (posIndex < nodeAnim->mNumPositionKeys ||
				rotIndex < nodeAnim->mNumRotationKeys ||
				scaleIndex < nodeAnim->mNumScalingKeys)
			{
				double smallestTime = 2 * outAnim.duration;
				if (posIndex < nodeAnim->mNumPositionKeys)
				{
					double localTime = nodeAnim->mPositionKeys[posIndex].mTime;
					smallestTime = smallestTime < localTime ? smallestTime : localTime;
				}

				if (rotIndex < nodeAnim->mNumRotationKeys)
				{
					double localTime = nodeAnim->mRotationKeys[rotIndex].mTime;
					smallestTime = smallestTime < localTime ? smallestTime : localTime;
				}

				if (scaleIndex < nodeAnim->mNumScalingKeys)
				{
					double localTime = nodeAnim->mScalingKeys[scaleIndex].mTime;
					smallestTime = smallestTime < localTime ? smallestTime : localTime;
				}

				AnimationKey outKey;
				outKey.time = smallestTime;

				if (posIndex < nodeAnim->mNumPositionKeys && nodeAnim->mPositionKeys[posIndex].mTime == smallestTime)
				{
					outKey.sqtMask |= TRANSLATION_MASK;
					outKey.trans[0] = nodeAnim->mPositionKeys[posIndex].mValue.x;
					outKey.trans[1] = nodeAnim->mPositionKeys[posIndex].mValue.y;
					outKey.trans[2] = nodeAnim->mPositionKeys[posIndex].mValue.z;

					if (m_settings.animation.bCreateAdditive && m_modelReference && cAnimIndex < m_modelReference->m_animations.size())
					{
						Animation& refAnim = m_modelReference->m_animations[cAnimIndex];
						if (i < refAnim.nodes.size() && posIndex < refAnim.nodes[i].keys.size())
						{
							float* refTrans = refAnim.nodes[i].keys[posIndex].trans;

							outKey.trans[0] = outKey.trans[0] - refTrans[0];
							outKey.trans[1] = outKey.trans[1] - refTrans[1];
							outKey.trans[2] = outKey.trans[2] - refTrans[2];
						}
					}

					posIndex++;
				}

				if (rotIndex < nodeAnim->mNumRotationKeys && nodeAnim->mRotationKeys[rotIndex].mTime == smallestTime)
				{
					outKey.sqtMask |= QUAT_MASK;
					outKey.quat[0] = nodeAnim->mRotationKeys[rotIndex].mValue.x;
					outKey.quat[1] = nodeAnim->mRotationKeys[rotIndex].mValue.y;
					outKey.quat[2] = nodeAnim->mRotationKeys[rotIndex].mValue.z;
					outKey.quat[3] = nodeAnim->mRotationKeys[rotIndex].mValue.w;

					if (m_settings.animation.bCreateAdditive && m_modelReference && cAnimIndex < m_modelReference->m_animations.size())
					{
						Animation& refAnim = m_modelReference->m_animations[cAnimIndex];
						if (i < refAnim.nodes.size() && posIndex < refAnim.nodes[i].keys.size())
						{
							float* refRot = refAnim.nodes[i].keys[rotIndex].quat;

							aiQuaternion refQuat(refRot[3], refRot[0], refRot[1], refRot[2]);
							aiQuaternion resQuat = nodeAnim->mRotationKeys[rotIndex].mValue * refQuat.Conjugate();

							outKey.quat[0] = resQuat.x;
							outKey.quat[1] = resQuat.y;
							outKey.quat[2] = resQuat.z;
							outKey.quat[3] = resQuat.w;
						}
					}

					rotIndex++;
				}

				if (scaleIndex < nodeAnim->mNumScalingKeys && nodeAnim->mScalingKeys[scaleIndex].mTime == smallestTime)
				{
					outKey.sqtMask |= SCALE_MASK;
					outKey.scale[0] = nodeAnim->mScalingKeys[scaleIndex].mValue.x;
					outKey.scale[1] = nodeAnim->mScalingKeys[scaleIndex].mValue.y;
					outKey.scale[2] = nodeAnim->mScalingKeys[scaleIndex].mValue.z;

					if (m_settings.animation.bCreateAdditive && m_modelReference && cAnimIndex < m_modelReference->m_animations.size())
					{
						Animation& refAnim = m_modelReference->m_animations[cAnimIndex];
						if (i < refAnim.nodes.size() && posIndex < refAnim.nodes[i].keys.size())
						{
							float* refScale = refAnim.nodes[i].keys[scaleIndex].scale;

							outKey.scale[0] = outKey.scale[0] / refScale[0];
							outKey.scale[1] = outKey.scale[1] / refScale[1];
							outKey.scale[2] = outKey.scale[2] / refScale[2];
						}
					}

					scaleIndex++;
				}

				outNode.keys.push_back(outKey);
			}
		}
	}

	void ModelParser::setModelReference(ModelParser* modelReference)
	{
		m_modelReference = modelReference;
	}

	aiNode* ModelParser::findNodeByName(std::string name)
	{
		for (unsigned int i = 0; i < m_aiNodes.size(); i++)
		{
			if (name == m_aiNodes[i]->mName.data)
			{
				return m_aiNodes[i];
			}
		}
		return nullptr;
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