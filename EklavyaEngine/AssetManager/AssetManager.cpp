#include "AssetManager.h"

#include <cassert>
using namespace Eklavya::Asset;
using namespace Eklavya;

AssetManager::AssetManager() : Singleton()
{
}

AssetManager::~AssetManager()
{
	if (!mAssetMap.empty())
		RemoveAll();
}

SHARED_TEXTURE AssetManager::LoadTexture(std::string assetName, std::string ext, bool repeat)
{
	SHARED_TEXTURE newAsset = AssetFactory::CreateTexture(assetName, ext, repeat);
	if (newAsset != nullptr)
		mAssetMap[EType::TEXTURE].push_back(newAsset);
	return newAsset;
}

SHARED_SHADER AssetManager::LoadShader(std::string shaderName)
{
	SHARED_SHADER newAsset = AssetFactory::CreateShader(shaderName);
	mAssetMap[EType::SHADER].push_back(newAsset);
	return newAsset;
}

SHARED_CUBEMAP AssetManager::LoadCubemap(std::string folderName, std::string ext)
{
	SHARED_CUBEMAP newAsset = AssetFactory::CreateCubemap(folderName, ext);
	mAssetMap[EType::CUBEMAP].push_back(newAsset);
	return newAsset;
}

SHARED_MODEL AssetManager::LoadModel(std::string folderName, std::string ext, int modelID)
{
	SHARED_MODEL newAsset = AssetFactory::CreateModel(folderName, ext, modelID);
	mAssetMap[EType::MODEL].push_back(newAsset);
	return newAsset;
}

SHARED_ANIMATION AssetManager::LoadAnimation(std::string animName, std::string ext, int modelID)
{
	SHARED_ANIMATION newAsset = AssetFactory::CreateAnimation(animName, ext, modelID);
	mAssetMap[EType::ANIMATION].push_back(newAsset);
	return newAsset;
}

void AssetManager::RemoveAsset(EType type, const std::string& name)
{
	auto listEnd = mAssetMap[type].end();
	auto iter = std::find_if(mAssetMap[type].begin(), listEnd, [&](std::shared_ptr<IAsset> asset) { return (name == asset->GetName()); });
	if (iter != listEnd)
		mAssetMap[type].erase(iter);
}

void AssetManager::RemoveAll()
{
	for (int type = 0; type < 5; ++type)
	{
		auto& list = mAssetMap[(EType)type];
		list.clear();
	}
	mAssetMap.clear();
}
