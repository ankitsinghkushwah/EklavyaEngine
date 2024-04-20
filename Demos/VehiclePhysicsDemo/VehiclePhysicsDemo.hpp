//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_VEHICLE_PHYSICS_DEMO_H_
#define INC_VEHICLE_PHYSICS_DEMO_H_

#include <MainEntryScene.hpp>
#include <UserInputListener.h>
#include "Material.h"
#include "MaterialInfo.h"
#include "CarSuspension.hpp"
#include <SFML/Audio.hpp>
#include "Scene/Cameras/SpringFolllowCamera.h"

namespace Eklavya
{
  class VehiclePhysicsDemo final : public MainEntryScene
  {
  public:
    VehiclePhysicsDemo(class Director *pDirector);

    ~VehiclePhysicsDemo() override;

  private:
    void                                CreateCubeStack();

    std::array<SHARED_SUSPENSION, 4>    mSuspensions;

    Physics::SHARED_EKBODY              mChassisBody = nullptr;

    void                                PreloadTextures();

    Asset::MaterialInfo LoadMaterialInfo(const std::string &file,
                                         std::string        ext = "png");
    void                ImGuiProc() override;

    void                CreateStage();

    void                CreateCar();

    void                LoadMesh();

#ifdef EKDEBUG
    void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;
#endif
  };

} // namespace Eklavya

#endif
