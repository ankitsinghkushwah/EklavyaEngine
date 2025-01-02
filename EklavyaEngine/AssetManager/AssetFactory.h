#pragma once
#include <string>
#include <memory>
#include "Asset.h"
namespace Eklavya::Asset
{
	class IAsset;
	class AssetFactory
	{
	  public:
		static SHARED_TEXTURE    CreateTexture(std::string assetName, std::string ext, bool repeat = false);
		static SHARED_SHADER     CreateShader(std::string assetName);
		static SHARED_CUBEMAP    CreateCubemap(std::string folderName, std::string ext);
		static SHARED_MODEL      CreateModel(std::string assetName, std::string ext, int modelID); /*model id is used to read/write to
                     AnimationData which is shared by animation and model */
		static SHARED_ANIMATION  CreateAnimation(std::string assetName, std::string ext, int modelID);
		static const std::string s_TexturesDirPath;
		static const std::string s_CubemapsDirPath;
		static const std::string s_ShadersDirPath;
		static const std::string s_ModelsDirPath;
		static const std::string s_AnimationsDirPath;
	};
} // namespace Eklavya::Asset
