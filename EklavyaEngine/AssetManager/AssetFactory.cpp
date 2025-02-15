#include "AssetFactory.h"
#include "Texture2D.h"
#include <AssetManager/ShaderProgram.h>
#include "Cubemap.h"
#include <AssetManager/GLModel.hpp>
#include <vector>
#include "Animation.h"

using namespace Eklavya::Asset;
const std::string AssetFactory::s_TexturesDirPath = "Resources/textures/";
const std::string AssetFactory::s_CubemapsDirPath = "Resources/cubemap/";
const std::string AssetFactory::s_ShadersDirPath = "Resources/shaders/";
const std::string AssetFactory::s_ModelsDirPath = "Resources/models/";
const std::string AssetFactory::s_AnimationsDirPath = "Resources/models/";

SHARED_TEXTURE AssetFactory::CreateTexture(std::string assetName, std::string ext, bool repeat)
{
	SHARED_TEXTURE newTexture = std::make_shared<Texture2D>(assetName);
	int       created = newTexture->CreateTexture(s_TexturesDirPath + assetName + "." + ext, repeat);
	return created != -1 ? newTexture : nullptr;
}

SHARED_SHADER AssetFactory::CreateShader(std::string assetName)
{
	SHARED_SHADER shader = std::make_shared<ShaderProgram>(assetName);
	std::string   suffix = "";
	for (int type = 0; type < EShaderType::SHADER_TYPE_MAX; ++type)
	{
		switch (type)
		{
			case EShaderType::VERTEX:
				suffix = "_vs.glsl";
				break;
			case EShaderType::FRAGMENT:
				suffix = "_fs.glsl";
				break;
		};
		shader->AddAndCompile(s_ShadersDirPath + assetName + suffix, (EShaderType)type);
	}
	shader->Build();
	return shader;
}

SHARED_CUBEMAP AssetFactory::CreateCubemap(std::string folderName, std::string ext)
{
	std::vector<std::string> faceNames;
	faceNames.push_back(s_CubemapsDirPath + folderName + "/px." + ext);
	faceNames.push_back(s_CubemapsDirPath + folderName + "/nx." + ext);
	faceNames.push_back(s_CubemapsDirPath + folderName + "/py." + ext);
	faceNames.push_back(s_CubemapsDirPath + folderName + "/ny." + ext);
	faceNames.push_back(s_CubemapsDirPath + folderName + "/pz." + ext);
	faceNames.push_back(s_CubemapsDirPath + folderName + "/nz." + ext);
	SHARED_CUBEMAP cubeMap = std::make_shared<Cubemap>(folderName);
	cubeMap->Create(faceNames);
	return cubeMap;
}

SHARED_MODEL AssetFactory::CreateModel(std::string assetName, std::string ext, int modelID)
{
	std::string  path = s_ModelsDirPath + assetName + ext;
	SHARED_MODEL model = std::make_shared<GLModel>(assetName, modelID);
	model->Load(path);
	return model;
}

SHARED_ANIMATION AssetFactory::CreateAnimation(std::string assetName, std::string ext, int modelID)
{
	Assimp::Importer importer;
	std::string      path = s_ModelsDirPath + assetName + ext;
	const aiScene*   scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_OptimizeGraph);
	if (!scene || !scene->mRootNode)
	{
		std::string error = std::string(importer.GetErrorString());
		return nullptr;
	}
	SHARED_ANIMATION newAnimation = std::make_shared<Animation>(assetName, modelID, scene->mAnimations[0], scene);
	return newAnimation;
}
