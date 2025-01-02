#include "GLModel.hpp"
#include <string>
#include "Texture2D.h"
#include "AnimationData.h"

using namespace std;
using namespace Eklavya::Asset;

GLModel::GLModel(const std::string& assetName, int modelId) : IAsset(EType::MODEL, assetName), m_ModelID(modelId)
{
}

GLModel::~GLModel()
{
	for (GLMesh* mesh : mSceneMeshes)
		delete mesh;
	mSceneMeshes.clear();
}

void GLModel::Load(const std::string& path)
{
	Assimp::Importer importer;
	const aiScene*   scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenBoundingBoxes | aiProcess_FlipUVs);
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) // if is Not Zero
	{
		const char* str = importer.GetErrorString();
		LOG_STRING("ERROR::ASSIMP:: " + std::string(str));
		return;
	}
	mDirectory = path.substr(0, path.find_last_of("/"));
	LoadMaterials(scene);
	aiMatrix4x4 identity;

	if (scene->mRootNode)
	{
		glm::mat4 parentTransform = GetGLMMat(scene->mRootNode->mTransformation);
		mRootNode = ProcessNode(*scene->mRootNode, *scene, parentTransform);
	}
}

std::shared_ptr<ModelNode> GLModel::ProcessNode(aiNode& parentNode, const aiScene& scene, glm::mat4& parentTransform)
{
	std::shared_ptr<ModelNode> modelNode = std::make_shared<ModelNode>();
	modelNode->mName = parentNode.mName.C_Str();
	modelNode->mTransform = parentTransform;
	for (unsigned int i = 0; i < parentNode.mNumMeshes; i++)
	{
		const aiMesh* mesh = scene.mMeshes[parentNode.mMeshes[i]];
		std::string   name = mesh->mName.C_Str();
		assert(mesh);
		modelNode->mMeshes.push_back(ExtractMeshData(*mesh, scene));
	}
	for (unsigned int i = 0; i < parentNode.mNumChildren; i++)
	{
		aiNode*                    childAssimpNode = parentNode.mChildren[i];
		glm::mat4                  childTransform = glm::mat4(1.0f);
		std::shared_ptr<ModelNode> childNode = ProcessNode(*childAssimpNode, scene, childTransform);
		modelNode->mChildren.push_back(childNode);
	}
	return modelNode;
}

GLMesh* GLModel::ExtractMeshData(const aiMesh& mesh, const aiScene& scene)
{
	MeshVAOData meshData;
	for (unsigned int i = 0; i < mesh.mNumVertices; i++)
	{
		glm::vec3 vertex = GetGLMVec(mesh.mVertices[i]);
		meshData.vertices.push_back(vertex.x);
		meshData.vertices.push_back(vertex.y);
		meshData.vertices.push_back(vertex.z);
		glm::vec3 normal = GetGLMVec(mesh.mNormals[i]);
		meshData.normals.push_back(normal.x);
		meshData.normals.push_back(normal.y);
		meshData.normals.push_back(normal.z);
		if (mesh.mTextureCoords[0])
		{
			meshData.uvs.push_back(mesh.mTextureCoords[0][i].x);
			meshData.uvs.push_back(mesh.mTextureCoords[0][i].y);
		}

		for (int i = 0; i < MAX_BONE_INFLUENCE; ++i)
		{
			meshData.boneIds.push_back(-1.0f);
			meshData.weights.push_back(0.0f);
		}
	}
	for (unsigned int i = 0; i < mesh.mNumFaces; i++)
	{
		aiFace face = mesh.mFaces[i];
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			meshData.indices.push_back(face.mIndices[j]);
	}

	std::vector<BoneInfo> meshBones;
	ExtractBoneWeightForVertices(meshData, meshBones, mesh, scene);
	GLMesh* glMesh = new GLMesh();
	glMesh->mName = mesh.mName.C_Str();
	glMesh->InitVAO(std::move(meshData));
	glMesh->mMaterialInfo = mMaterials[mesh.mMaterialIndex];
	glMesh->mBones = std::move(meshBones);
	glMesh->mMin = GetGLMVec(mesh.mAABB.mMin);
	glMesh->mMax = GetGLMVec(mesh.mAABB.mMax);
	return glMesh;
}

void GLModel::LoadMaterials(const aiScene* pScene)
{
	auto loadTexture = [&](aiTextureType texType, const aiMaterial* material, bool firstIndex = true) -> SHARED_TEXTURE
	{
		SHARED_TEXTURE tex = nullptr;
		aiString       aiPath;
		material->Get(AI_MATKEY_TEXTURE(texType, 0), aiPath);
		std::string path = aiPath.C_Str();

		tex = std::make_shared<Texture2D>();
		path = mDirectory + "/" + path;
		bool loaded = tex->CreateTexture(path) != -1;
		if (loaded == true)
		{
			return tex;
		}
		return nullptr;
	};
	for (uint32_t mat_index = 0; mat_index < pScene->mNumMaterials; ++mat_index)
	{
		const aiMaterial* material = pScene->mMaterials[mat_index];
		MaterialInfo      matInfo;
		matInfo.mBaseColor = glm::vec3(0.6f);
		matInfo.mOpacity = 1.0f;
		matInfo.mMetallic = 0.0f;
		matInfo.mRoughness = 1.0f;
		matInfo.mBaseMap = loadTexture(aiTextureType_DIFFUSE, material);
		matInfo.mNormalMap = loadTexture(aiTextureType_NORMALS, material);
		matInfo.mAOMap = loadTexture(aiTextureType_AMBIENT, material);
		matInfo.mRoughMap = loadTexture(aiTextureType_METALNESS, material); // roughness
		matInfo.mMetalMap = loadTexture(aiTextureType_DIFFUSE_ROUGHNESS, material,
		                                false); // metallic
		mMaterials.push_back(matInfo);
	}
}

void GLModel::SetVertexBoneData(MeshVAOData& meshData, int vertexIdx, int boneID, float weight)
{
	int boneDataStartIndex = vertexIdx * 4;

	for (int i = 0; i < 4; ++i)
	{
		int idx = boneDataStartIndex + i;

		if (meshData.boneIds[idx] < 0)
		{
			meshData.boneIds[idx] = boneID;
			meshData.weights[idx] = weight;
			break;
		}
	}
}

void GLModel::ExtractBoneWeightForVertices(MeshVAOData& meshData, std::vector<BoneInfo>& meshBones, const aiMesh& mesh, const aiScene& scene)
{
	auto& modelDataMap = ModelsBoneData::s_BonesDataMap[m_ModelID];
	auto& boneInfoMap = modelDataMap.m_BoneInfoMap;
	int&  boneCount = modelDataMap.m_BoneCount;

	for (int boneIndex = 0; boneIndex < mesh.mNumBones; ++boneIndex)
	{
		auto&       bone = mesh.mBones[boneIndex];
		std::string boneName = bone->mName.C_Str();
		int         boneID = -1;
		if (boneInfoMap.find(boneName) == boneInfoMap.end())
		{
			BoneInfo newBoneInfo;
			newBoneInfo.id = boneCount;
			newBoneInfo.offset = GetGLMMat(mesh.mBones[boneIndex]->mOffsetMatrix);
			boneInfoMap[boneName] = newBoneInfo;
			boneID = boneCount;
			boneCount++;
		}
		else
		{
			boneID = boneInfoMap[boneName].id;
		}

		meshBones.push_back(boneInfoMap[boneName]);
		auto weights = mesh.mBones[boneIndex]->mWeights;
		int  numWeights = mesh.mBones[boneIndex]->mNumWeights;

		for (int weightIndex = 0; weightIndex < numWeights; ++weightIndex)
		{
			int   vertexId = weights[weightIndex].mVertexId;
			float weight = weights[weightIndex].mWeight;
			SetVertexBoneData(meshData, vertexId, boneID, weight);
		}
	}
}
