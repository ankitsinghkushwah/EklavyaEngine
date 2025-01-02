////
////  BaseDemoScene.cpp
////  EklavyaEngine
////
////  Created by Ankit Singh Kushwah on 19/09/22.
////
//
//#include "FPSDemo.hpp"
//#include <InputHandler.h>
//#include "RenderComponent.hpp"
//#include "EkActor.h"
//#include "AssetManager.h"
//#include "Material.h"
//#include "imgui.h"
//#include "Random.h"
//#include "EkPhysics/EkBody.h"
//#include "EkPhysics/Collider.h"
//#include <memory>
//#include "FreeLookCamera.h"
//#include "CarComponent.hpp"
//#include "Animation.h"
//
//using namespace Eklavya::Asset;
//using namespace Eklavya::Renderer;
//using namespace Eklavya::Physics;
//
//namespace Eklavya {
//
//void FPSDemo::PreloadTextures() {
//
//
//
//    std::string ext = "png";
//
//    for (auto &name : mCharacters) {
//        std::string folder =  "characters/" + name + "/";
//
//        AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
//    }
//
//    std::vector<std::string> texturesList = {"grid", "crate", "grass", "floor"};
//
//    for (auto &name : texturesList) {
//        std::string folder = "pbr/" + name + "/";
//
//        AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
//        AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
//    }
//}
//
//MaterialInfo FPSDemo::LoadMaterialInfo(const std::string &file,
//                                                     std::string ext) {
//    MaterialInfo info;
//
//    std::string folder = file + "/";
//    info.mBaseMap =
//    AssetManager::GetInstance().GetAsset<Texture2D>(folder + "albedo");
//    info.mNormalMap =
//    AssetManager::GetInstance().GetAsset<Texture2D>(folder + "normal");
//    info.mAOMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "ao");
//    info.mRoughMap =
//    AssetManager::GetInstance().GetAsset<Texture2D>(folder + "roughness");
//    info.mMetalMap =
//    AssetManager::GetInstance().GetAsset<Texture2D>(folder + "metallic");
//
//
//    return info;
//}
//
//void FPSDemo::CreateStage() {
//    MaterialInfo info = LoadMaterialInfo("pbr/grid");
//    info.mRoughness = 1.0f;
//    info.mTiling = 40;
//
//    float area_extent = 10000;
//    float floorScaleY = 5.0f;
//    CreateCube(glm::vec3(0.0f), glm::vec3(area_extent, floorScaleY, area_extent),
//               glm::vec3(), FLT_MAX, info, STATIC);
//}
//
//
//std::string FPSDemo::GetExtension(const std::string& character)
//{
//    if(character == "demo_soldier")
//    {
//        return ".fbx";
//    }
//    else if(character == "swat")
//    {
//        return ".dae";
//    }
//
//    return ".fbx";
//}
//
//void FPSDemo::LoadCharactersAndAnimations()
//{
//    int modelID = 0;
//    for(std::string& character : mCharacters)
//    {
//        std::string ext = GetExtension(character);
//        AssetManager::GetInstance().LoadModel(ModelName(character), ext, modelID);
//        std::vector<std::string> animations = AnimationsToLoad(character);
//        for(auto& animation : animations)
//        {
//            AssetManager::GetInstance().LoadAnimation(AnimationName(character, animation),ext, modelID);
//        }
//        modelID++;
//    }
//}
//
//SceneHelper::ModelLoadOptions FPSDemo::GetMaterialsForCharacter(const std::string &character)
//{
//    SceneHelper::ModelLoadOptions loadOptions;
//    SceneHelper::MATERIALS_FOR_ACTOR materials;
//    if(character == "demo_soldier")
//    {
//        loadOptions.forceSingleMaterial = true;
//        Asset::MaterialInfo materialInfo = LoadMaterialInfo("characters/"+character);
//        materialInfo.mMeshName = "";
//        materials.push_back(materialInfo);
//    }
//    loadOptions.materials = std::move(materials);
//    return loadOptions;
//}
//
//std::vector<std::string> FPSDemo::AnimationsToLoad(const std::string& character)
//{
//    std::vector<std::string> animationsToLoad;
//
//    if(character == "demo_soldier")
//    {
////        animationsToLoad.push_back("idle");
////        animationsToLoad.push_back("run");
//        animationsToLoad.push_back("death");
//    }
//    if(character == "swat")
//    {
//        animationsToLoad.push_back("idle");
//        animationsToLoad.push_back("run");
//    }
//
//     if(character == "new_soldier")
//    {
//        animationsToLoad.push_back("idle");
//    }
//
//    return animationsToLoad;
//}
//
//void FPSDemo::InstantiateCharacters()
//{
//    float xOff = 100.0f;
//    int i = 1;
//    int modelID = 0;
//  for(std::string& character : mCharacters)
//  {
//
//      SceneHelper::ModelLoadOptions materials = GetMaterialsForCharacter(character);
//
//      std::vector<std::string> animationsToLoad = AnimationsToLoad(character);
//
//      SHARED_ACTOR actor = SceneHelper::CreateActorFromModel(ModelName(character),modelID,materials);
//      SHARED_ANIMATOR animator = actor->EmplaceComponent<AnimationComponent>(modelID);
//      animator->Init();
//
//      for(const std::string& animation : animationsToLoad)
//      {
//          SHARED_ANIMATION animAsset =
//          AssetManager::GetInstance().GetAsset<Asset::Animation>(AnimationName(character, animation));
//          mAnimations[character].push_back(animAsset);
//      }
//      animator->PlayAnimation(mAnimations[character][0]);
//      actor->Transform()->SetPosition(i * xOff, 2.0f, 0.0f);
//      actor->Transform()->SetScale(1.0f);
//      AddActor(actor);
//
//      i++;
//      modelID++;
//  }
//}
//
//FPSDemo::FPSDemo(Director *pDirector)
//: MainEntryScene(pDirector) {
//
//   // mCharacters.push_back("swat");
//
//    mCharacters.push_back("demo_soldier");
//
//    PreloadTextures();
//    LoadCharactersAndAnimations();
//    CreateStage();
//    InstantiateCharacters();
//}
//
//FPSDemo::~FPSDemo() {}
//
//void FPSDemo::ImGuiProc() { MainEntryScene::ImGuiProc(); }
//
//#ifdef EKDEBUG
//void FPSDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer) {
//    MainEntryScene::DebugDraw(debugRenderer);
//}
//#endif
//
//} // namespace Eklavya
