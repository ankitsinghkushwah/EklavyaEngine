//
//  EkSceneHelper.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/07/2023.
//

#ifndef EkSceneHelper_hpp
#define EkSceneHelper_hpp

#include <stdio.h>

#include "GLModel.hpp"
#include "AssetManager.h"
#include "EkActor.h"
#include "MaterialInfo.h"

namespace Eklavya::SceneHelper {

using MATERIALS_FOR_ACTOR = std::vector<Asset::MaterialInfo>;

struct ModelLoadOptions
{
    bool forceSingleMaterial = false;
    MATERIALS_FOR_ACTOR materials;
};

SHARED_ACTOR CreateActorFromModel(const std::string &name, int modelID, ModelLoadOptions loadOptions = {});

SHARED_ACTOR CreateActorForMesh(const Asset::GLMesh &glMesh,
                                const Asset::ModelNode &node, int modelID);

std::optional<Asset::MaterialInfo> GetMaterialForMesh(const std::string& name,const MATERIALS_FOR_ACTOR& materials);

SHARED_ACTOR ModelToActor(std::shared_ptr<Asset::ModelNode> node, int modelID,const ModelLoadOptions& materials);
} // namespace Eklavya::SceneHelper

#endif /* EkSceneHelper_hpp */
