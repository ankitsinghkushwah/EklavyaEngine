#pragma once
/*
        Keeps all assets of different type in a map.
        different systems will ask for asset when required
        will ownership of asset will be given to the system.
        The system which using the asset SHOULD GIVE UP ownership
        when not using the asset so It can be destroyed by AssetManager in
        the end.
*/
#include "../CoreUtils/Logger.h"
#include "../CoreUtils/Singleton.h"
#include "Animation.h"
#include "Asset.h"
#include "AssetFactory.h"
#include "Cubemap.h"
#include "IAsset.h"
#include "Texture2D.h"
#include <AssetManager/GLModel.hpp>
#include <AssetManager/ShaderProgram.h>
#include <algorithm>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
namespace Eklavya
{
	using AssetList = std::list<std::shared_ptr<Asset::IAsset>>;
	class AssetManager : public Singleton<AssetManager>
	{
	  public:
		AssetManager();
		~AssetManager();
		SHARED_TEXTURE   LoadTexture(std::string assetName, std::string ext, bool repeat = false);
		SHARED_SHADER    LoadShader(std::string shaderName);
		SHARED_CUBEMAP   LoadCubemap(std::string folderName, std::string ext);
		SHARED_MODEL     LoadModel(std::string folderName, std::string ext, int modelID);
		SHARED_ANIMATION LoadAnimation(std::string animName, std::string ext, int modelID);

		template<typename Type>
		std::shared_ptr<Type> GetAsset(const std::string& name)
		{
			Asset::EType type = Asset::EType::ASSET_TYPE_MAX;
			if (std::is_same<Type, Asset::Texture2D>::value)
				type = Asset::EType::TEXTURE;
			else if (std::is_same<Type, Asset::ShaderProgram>::value)
				type = Asset::EType::SHADER;
			else if (std::is_same<Type, Asset::Cubemap>::value)
				type = Asset::EType::CUBEMAP;
			else if (std::is_same<Type, Asset::GLModel>::value)
				type = Asset::EType::MODEL;
			else if (std::is_same<Type, Asset::Animation>::value)
				type = Asset::EType::ANIMATION;

			assert(type != Asset::EType::ASSET_TYPE_MAX);

			auto listEnd = mAssetMap[type].end();
			auto iter = std::find_if(mAssetMap[type].begin(), listEnd, [&](std::shared_ptr<Asset::IAsset> asset) { return name == asset->GetName(); });
			if (iter != listEnd)
				return std::static_pointer_cast<Type>(*iter);
			else
				return std::shared_ptr<Type>(nullptr);
		}
		void RemoveAsset(Asset::EType type, const std::string& name);
		void RemoveAll();

	  private:
		std::map<Asset::EType, AssetList> mAssetMap;
	};
} // namespace Eklavya
