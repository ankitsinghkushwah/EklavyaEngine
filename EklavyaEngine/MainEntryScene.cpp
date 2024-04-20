//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "MainEntryScene.hpp"

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
#include "SceneHelper.hpp"
#include "SpringFollowCamera.hpp"

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya {
enum MatIndex { GOLD, RUBBER, CRATE_SCIFI, CRATE_WOOD, PLASTIC };

SHARED_ACTOR MainEntryScene::CreateCube(glm::vec3 pos, glm::vec3 scale,
                                        glm::vec3 rotate, float mass,
                                        Asset::MaterialInfo matInfo,
                                        uint32_t flag, bool kid) {
  SHARED_ACTOR floor = std::make_shared<EkActor>();
  floor->Transform()->SetScale(scale);
  floor->Transform()->SetPosition(pos);
  auto render = floor->EmplaceComponent<RenderComponent>(EMeshType::CUBE);
  render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
  render->GetMesh().mMaterialInfo = matInfo;

  if (!kid) {
    auto collider = std::make_shared<Physics::BoxCollider>();
    collider->SetHalfSize(floor->Transform()->GetScale());
    collider->SetGroupIndex(flag);

    auto body =
        floor->EmplaceComponent<Physics::EkBody>(floor->Transform(), collider);
    body->SetMass(mass);
    body->SetPos(pos);
    body->SetRotation(rotate);

    collider->SetBody(body.get());

    AddActor(floor);
  }

  return floor;
}

SHARED_ACTOR MainEntryScene::CreateSphere(glm::vec3 pos, float radius,
                                          float mass,
                                          Asset::MaterialInfo matInfo,
                                          uint32_t flag, bool kid) {
  SHARED_ACTOR sphere = std::make_shared<EkActor>();
  sphere->Transform()->SetScale(radius);
  sphere->Transform()->SetPosition(pos);
  auto render = sphere->EmplaceComponent<RenderComponent>(EMeshType::SPHERE);
  render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
  render->GetMesh().mMaterialInfo = matInfo;

  if (!kid) {
    auto collider = std::make_shared<Physics::SphereCollider>();
    collider->SetRadius(radius);
    collider->SetGroupIndex(flag);

    auto body =
        sphere->EmplaceComponent<Physics::EkBody>(sphere->Transform(), collider);
    body->SetMass(mass);
    body->SetPos(pos);
    body->SetRestitution(.4f);
    collider->SetBody(body.get());

    AddActor(sphere);
  }

  return sphere;
}

MainEntryScene::MainEntryScene(Director *pDirector) : EkScene(pDirector) {
  mRenderer->GetMaterialForGroup<SkyboxMaterial>(Renderer::ERenderGroup::SKYBOX)
      ->mCubemap = AssetManager::GetInstance().LoadCubemap("Day", "png");

  SHARED_ACTOR sky = std::make_shared<EkActor>();
  SHARED_RENDERCOMPONENT render =
      sky->EmplaceComponent<RenderComponent>(EMeshType::CUBE);
  render->mProjectsShadow = false;
  render->mRenderGroup = Renderer::ERenderGroup::SKYBOX;
  AddActor(sky);
}

MainEntryScene::~MainEntryScene() {}

void MainEntryScene::ImGuiProc() { EkScene::ImGuiProc(); }

#ifdef EKDEBUG
void MainEntryScene::DebugDraw(Renderer::DebugRenderer &debugRenderer) {
  EkScene::DebugDraw(debugRenderer);
}
#endif

} // namespace Eklavya
