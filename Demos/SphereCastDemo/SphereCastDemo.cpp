//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "SphereCastDemo.hpp"
#include <InputHandler.h>
#include "RenderComponent.hpp"
#include "EkActor.h"
#include "AssetManager.h"
#include "Material.h"
#include "imgui.h"
#include "Random.h"
#include "EkPhysics/EkBody.h"
#include "EkPhysics/Collider.h"
#include <memory>
#include "FreeLookCamera.h"
#include "CarComponent.hpp"
#include "Animation.h"
#include <glfw/glfw3.h>

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{

  void SphereCastDemo::PreloadTextures()
  {

    std::string              ext = "png";

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

  MaterialInfo SphereCastDemo::LoadMaterialInfo(const std::string &file,
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

  void SphereCastDemo::CreateStage()
  {
        MaterialInfo info = LoadMaterialInfo("pbr/grid");
        info.mRoughness = 1.0f;
        info.mTiling = 40;
    
        float area_extent = 10000;
        float floorScaleY = 5.0f;
        CreateCube(glm::vec3(0.0f),
                   glm::vec3(area_extent, floorScaleY, area_extent),
                   glm::vec3(), FLT_MAX, info, STATIC);

    MaterialInfo info2;
    info2.mBaseColor = glm::vec3(1.0f);
    info2.mRoughness = 1.0f;
    info2.mTiling = 1.0f;
    CreateCube(glm::vec3(0.0, 13, -100.0f), glm::vec3(100.0f, 7.0f, 120),
               glm::vec3(glm::radians(15.0f), 0.0f, 0.0f), FLT_MAX, info,
               0);

    CreateSphere(glm::vec3(-200.0f, 50.0f, 0.0f), 40.0f, FLT_MAX, info2, STATIC);
  }

  SphereCastDemo::SphereCastDemo(Director *pDirector)
      : MainEntryScene(pDirector)
  {

    PreloadTextures();

    CreateStage();
  }

  SphereCastDemo::~SphereCastDemo() {}

  void SphereCastDemo::OnMouseAction(int key, int action)
  {
    if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
      {
        mRayStart = CurrentCamera()->Position();
        mRayDirection = CurrentCamera()->Forward();
        mRayRange = 300000.0f;

        mLastCastHitResult =
            mPhysicsWorld->RayCast(mRayStart, mRayDirection, mRayRange, -1);
      }
  }

  void SphereCastDemo::ImGuiProc() { MainEntryScene::ImGuiProc(); }

#ifdef EKDEBUG
  void SphereCastDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer)
  {
    MainEntryScene::DebugDraw(debugRenderer);

    glm::vec3 endPoint = mLastCastHitResult.success
                             ? mLastCastHitResult.position
                             : mRayStart + mRayDirection * mRayRange;
    debugRenderer.DrawLine(mRayStart, endPoint,
                           glm::vec4(0.0f, 0.0f, 0.0f, .7f), .2f);
    debugRenderer.DrawSphere(endPoint, glm::vec3(0.0f), 3.0f,
                             glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
  }
#endif

} // namespace Eklavya
