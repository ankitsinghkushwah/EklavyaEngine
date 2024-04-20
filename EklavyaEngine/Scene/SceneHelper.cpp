//
//  EkSceneHelper.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/07/2023.
//

#include "SceneHelper.hpp"
#include "RenderComponent.hpp"
#include "Material.h"
#include <glm/gtx/matrix_decompose.hpp>
using namespace Eklavya;
using namespace Eklavya::Asset;

namespace Eklavya::SceneHelper
{

  SHARED_ACTOR CreateActorFromModel(const std::string &name, int modelId,
                                    ModelLoadOptions materials)
  {
    SHARED_MODEL model = AssetManager::GetInstance().GetAsset<GLModel>(name);

    if (model == nullptr)
      {
        return nullptr;
      }
    ModelNode    dummy;
    SHARED_ACTOR actor = ModelToActor(model->mRootNode, modelId, materials);

    for (auto &mesh : model->mSceneMeshes)
      {
        actor->AddKid(
            CreateActorForMesh(*mesh, *model->mRootNode.get(), modelId));
      }

    return actor;
  }

  SHARED_ACTOR CreateActorForMesh(const GLMesh           &mesh,
                                  const Asset::ModelNode &node, int modelID)
  {
    SHARED_ACTOR actorWithMesh = std::make_shared<EkActor>();
    actorWithMesh->SetName(mesh.mName);
    auto render = actorWithMesh->EmplaceComponent<RenderComponent>(modelID);
    render->SetMesh(mesh);

    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;

    glm::vec3 dummy;
    glm::vec4 dumm2;

    glm::decompose(node.mTransform, scale, rotation, position, dummy, dumm2);

    actorWithMesh->Transform()->SetPosition(position.x, position.y, position.z);
    actorWithMesh->Transform()->SetRotation(rotation);
    actorWithMesh->Transform()->SetScale(scale.x, scale.y, scale.z);

    render->mModelID = modelID;
    render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
    render->mHasBones = mesh.mBones.empty() == false;

    return actorWithMesh;
  };

  std::optional<Asset::MaterialInfo>
  GetMaterialForMesh(const std::string         &name,
                     const MATERIALS_FOR_ACTOR &materials)
  {
    std::optional<MaterialInfo> materialInfo = std::nullopt;
    for (const Asset::MaterialInfo &info : materials)
      {
        if (name.compare(info.mMeshName) > 4)
          {
            materialInfo = info;
          }
      }
    return materialInfo;
  }

  SHARED_ACTOR ModelToActor(std::shared_ptr<Asset::ModelNode> node, int modelId,
                            const ModelLoadOptions &loadOptions)
  {
    SHARED_ACTOR actor = std::make_shared<EkActor>();

    actor->SetName(node->mName);

    for (auto &mesh : node->mMeshes)
      {
        if (loadOptions.forceSingleMaterial)
          {
            mesh->mMaterialInfo = loadOptions.materials[0];
          }
        else
          {
            std::optional<Asset::MaterialInfo> materialInfo =
                GetMaterialForMesh(mesh->mName, loadOptions.materials);
            if (materialInfo.has_value())
              {
                mesh->mMaterialInfo = materialInfo.value();
              }
          }

        actor->AddKid(CreateActorForMesh(*mesh, *node.get(), modelId));
      }

    for (auto &kid : node->mChildren)
      actor->AddKid(ModelToActor(kid, modelId, loadOptions));

    return actor;
  }

} // namespace Eklavya::SceneHelper
