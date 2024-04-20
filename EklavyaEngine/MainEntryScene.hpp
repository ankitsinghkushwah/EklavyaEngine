//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef BaseDemoScene_h
#define BaseDemoScene_h

#include <Scene/EkScene.h>
#include <UserInputListener.h>
#include "Material.h"
#include "MaterialInfo.h"
#include "PhysicsWorld.h"

namespace Eklavya {

class MainEntryScene : public EkScene {
public:
  MainEntryScene(class Director *pDirector);
  ~MainEntryScene() override;

  void ImGuiProc() override;

  void CreateStage();

  SHARED_ACTOR CreateCube(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate,
                          float mass, Asset::MaterialInfo matInfo,
                          uint32_t groupFlag = Physics::OTHER_COLLIDABLES,
                          bool kid = false);

  SHARED_ACTOR CreateSphere(glm::vec3 pos, float radius, float mass,
                            Asset::MaterialInfo matInfo,
                            uint32_t groupFlag = Physics::OTHER_COLLIDABLES,
                            bool kid = false);

#ifdef EKDEBUG
  void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;
#endif
};

} // namespace Eklavya

#endif /* BaseDemoScene_h */
