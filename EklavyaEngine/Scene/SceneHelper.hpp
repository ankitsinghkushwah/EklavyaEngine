//
//  EkSceneHelper.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/07/2023.
//

#ifndef EkSceneHelper_hpp
#define EkSceneHelper_hpp

#include <stdio.h>

#include <AssetManager/GLModel.hpp>
#include <AssetManager/AssetManager.h>
#include <Scene/EkActor.h>
#include <AssetManager/MaterialInfo.h>
#include <optional>

namespace Eklavya::SceneHelper
{
	using MATERIALS_FOR_ACTOR = std::vector<Asset::MaterialInfo>;

	struct ModelLoadOptions
	{
		bool forceSingleMaterial = false;
		MATERIALS_FOR_ACTOR materials;
	};

	UniqueActor CreateActorFromModel(const std::string &name, int modelID, ModelLoadOptions loadOptions = {});
} // namespace Eklavya::SceneHelper

#endif /* EkSceneHelper_hpp */
