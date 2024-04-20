//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "TPSDemo.hpp"

#include <InputHandler.h>
#include "RenderComponent.hpp"
#include "EkActor.h"
#include "AssetManager.h"
#include "Director.hpp"
#include "Material.h"
#include "imgui.h"
#include "Random.h"
#include "EkPhysics/EkBody.h"
#include "EkPhysics/Collider.h"
#include <memory>
#include "FreeLookCamera.h"
#include "CarComponent.hpp"
#include "Animation.h"
#include "PlayerController.hpp"

#include "Scene/Cameras/SpringFolllowCamera.h"

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{

  void TPSDemo::PreloadTextures()
  {

    std::string ext = "png";

    for (auto &name : mCharacters)
      {
        std::string folder = "characters/" + name + "/";

        AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
        AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
        AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
        AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
        AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
      }

    std::vector<std::string> texturesList = {"grid", "crate", "grass", "floor"};

    for (auto &name : texturesList)
      {
        std::string folder = "pbr/" + name + "/";

        AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
        AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
        AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
        AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
        AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
      }
  }

  MaterialInfo TPSDemo::LoadMaterialInfo(const std::string &file,
                                         std::string        ext)
  {
    MaterialInfo info;

    std::string  folder = file + "/";
    info.mBaseMap =
        AssetManager::GetInstance().GetAsset<Texture2D>(folder + "albedo");
    info.mNormalMap =
        AssetManager::GetInstance().GetAsset<Texture2D>(folder + "normal");
    info.mAOMap =
        AssetManager::GetInstance().GetAsset<Texture2D>(folder + "ao");
    info.mRoughMap =
        AssetManager::GetInstance().GetAsset<Texture2D>(folder + "roughness");
    info.mMetalMap =
        AssetManager::GetInstance().GetAsset<Texture2D>(folder + "metallic");

    return info;
  }

  void TPSDemo::CreateStage()
  {
    MaterialInfo info = LoadMaterialInfo("pbr/grid");
    info.mRoughness = 1.0f;
    info.mTiling = 40;

    float area_extent = 10000;
    float floorScaleY = 5.0f;
    CreateCube(glm::vec3(0.0f),
               glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(),
               FLT_MAX, info, 0);
  }

  std::string TPSDemo::GetExtension(const std::string &character)
  {
    if (character == "player")
      {
        return ".fbx";
      }
    else if (character == "swat")
      {
        return ".dae";
      }

    return ".fbx";
  }

  void TPSDemo::LoadCharactersAndAnimations()
  {
    int modelID = 0;
    for (std::string &character : mCharacters)
      {
        std::string ext = GetExtension(character);
        AssetManager::GetInstance().LoadModel(ModelName(character), ext,
                                              modelID);
        std::vector<std::string> animations = AnimationsToLoad(character);
        for (auto &animation : animations)
          {
            AssetManager::GetInstance().LoadAnimation(
                AnimationName(character, animation), ext, modelID);
          }
        modelID++;
      }
  }

  SceneHelper::ModelLoadOptions
  TPSDemo::GetMaterialsForCharacter(const std::string &character)
  {
    SceneHelper::ModelLoadOptions    loadOptions;
    SceneHelper::MATERIALS_FOR_ACTOR materials;
    if (character == "player")
      {
        loadOptions.forceSingleMaterial = true;
        Asset::MaterialInfo materialInfo =
            LoadMaterialInfo("characters/" + character);
        materialInfo.mMeshName = "";
        materials.push_back(materialInfo);
      }
    loadOptions.materials = std::move(materials);
    return loadOptions;
  }

  std::vector<std::string>
  TPSDemo::AnimationsToLoad(const std::string &character)
  {
    std::vector<std::string> animationsToLoad;

    if (character == "player")
      {
        // movement
        animationsToLoad.push_back("standing_movement/idle");
        animationsToLoad.push_back("standing_movement/walk_forward");
        animationsToLoad.push_back("standing_movement/walk_back");
        animationsToLoad.push_back("standing_movement/walk_left");
        animationsToLoad.push_back("standing_movement/walk_right");
        animationsToLoad.push_back("standing_movement/run_forward");
        animationsToLoad.push_back("standing_movement/run_back");
        animationsToLoad.push_back("standing_movement/run_left");
        animationsToLoad.push_back("standing_movement/run_right");

        // jumping

        animationsToLoad.push_back("jumping/takeoff");
        animationsToLoad.push_back("jumping/flight");
        animationsToLoad.push_back("jumping/land");
      }
    if (character == "swat")
      {
        animationsToLoad.push_back("idle");
        animationsToLoad.push_back("run");
      }

    if (character == "new_soldier")
      {
        animationsToLoad.push_back("idle");
      }

    return animationsToLoad;
  }

  void TPSDemo::InstantiateCharacters()
  {
    float xOff = 100.0f;
    int   i = 1;
    int   modelID = 0;
    for (std::string &character : mCharacters)
      {

        SceneHelper::ModelLoadOptions materials =
            GetMaterialsForCharacter(character);

        std::vector<std::string> animationsToLoad = AnimationsToLoad(character);

        SHARED_ACTOR             actor = SceneHelper::CreateActorFromModel(
            ModelName(character), modelID, materials);
        SHARED_ANIMATOR animator = actor->EmplaceComponent<Animator>(modelID);
        animator->Init();

        for (const std::string &animation : animationsToLoad)
          {
            SHARED_ANIMATION animAsset =
                AssetManager::GetInstance().GetAsset<Asset::Animation>(
                    AnimationName(character, animation));
            mAnimations[character].push_back(animAsset);
          }
        animator->PlayAnimation(mAnimations[character][0]);
        actor->Transform()->SetPosition(i * xOff, 2.0f, 0.0f);
        actor->Transform()->SetScale(1.0f);
        AddActor(actor);

        i++;
        modelID++;
      }
  }

  void TPSDemo::SetupPlayer()
  {

    std::string                   character = "player";
    int                           modelID = 0;

    SceneHelper::ModelLoadOptions materials =
        GetMaterialsForCharacter(character);
    std::vector<std::string> animationsToLoad = AnimationsToLoad(character);

    MaterialInfo             info;
    info.mBaseColor = glm::vec3(0.7f);
    info.mRoughness = 0.5f;
    info.mMetallic = 1.0f;

    glm::vec3 scale(1.0f);

    mPlayer = SceneHelper::CreateActorFromModel(ModelName(character), modelID,
                                                materials);
    SHARED_ANIMATOR animator = mPlayer->EmplaceComponent<Animator>(modelID);
    animator->Init();

    for (const std::string &animation : animationsToLoad)
      {
        SHARED_ANIMATION animAsset =
            AssetManager::GetInstance().GetAsset<Asset::Animation>(
                AnimationName(character, animation));
        mAnimations[character].push_back(animAsset);
      }

    animator->PlayAnimation(mAnimations[character][0]);

    mPlayer->Transform()->SetScale(scale);

    auto collider = std::make_shared<Physics::BoxCollider>();
    collider->SetHalfSize(glm::vec3(40.0f, 100, 40.0f));
    collider->SetGroupIndex(1);
    collider->SetOffset(glm::vec3(0.0f, 50.0f, 0.0f));

    SHARED_EKBODY body = mPlayer->EmplaceComponent<Physics::EkBody>(
        mPlayer->Transform(), collider);
    body->SetMass(70.0f);
    body->SetPos(glm::vec3(0.0f, 1000.0f, 0.0f));
    body->SetRotation(glm::vec3(0.0f));

    collider->SetBody(body.get());

    body->mGravityScale = 2.0f;
    body->SetLinearDamping(0.99);
    body->SetAngularDamping(0.99);
    body->SetDisableRotation(true);
    body->PutOnSleepOnCollision(true);
    body->SetAwake(true);

    mPlayerController = mPlayer->EmplaceComponent<PlayerController>(*this);
    mPlayerController->Init();

    AddActor(mPlayer);
  }

  TPSDemo::TPSDemo(Director *pDirector) : MainEntryScene(pDirector)
  {

    mCharacters.push_back("player");
    PreloadTextures();
    LoadCharactersAndAnimations();
    CreateStage();
    SetupPlayer();

    mFollowCamera = std::make_shared<SpringFollowCamera>(DefaulCameraParams());
    mFollowCamera->SetTarget(mPlayer->Transform());
    mFollowCamera->SetTargetOffset(glm::vec3(-40.0f, 100.0f, 0.0f));
    mFollowCamera->SetArmsLength(glm::vec3(0.0f, 100.0f, -200.0f));
    OverrideCamera(mFollowCamera);
  }

  TPSDemo::~TPSDemo() {}

  void TPSDemo::ImGuiProc()
  {
    MainEntryScene::ImGuiProc();
    mPlayerController->ImGuiProc();
  }

#ifdef EKDEBUG
  void TPSDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer)
  {
    MainEntryScene::DebugDraw(debugRenderer);
    mPlayerController->DebugDraw(debugRenderer);
  }
#endif

} // namespace Eklavya
