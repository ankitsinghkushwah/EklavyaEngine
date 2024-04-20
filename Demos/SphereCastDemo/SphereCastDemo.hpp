//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_SPHERE_CAST_DEMO
#define INC_SPHERE_CAST_DEMO

#include <MainEntryScene.hpp>
#include <UserInputListener.h>
#include "Material.h"
#include "MaterialInfo.h"
#include <SFML/Audio.hpp>
#include "SceneHelper.hpp"

namespace Eklavya
{
  class SphereCastDemo final : public MainEntryScene
  {
  public:
    SphereCastDemo(class Director *pDirector);

    ~SphereCastDemo() override;

  private:
    void                PreloadTextures();

    Asset::MaterialInfo LoadMaterialInfo(const std::string &file,
                                         std::string        ext = "png");
    void                ImGuiProc() override;

    void                CreateStage();
    
    void OnMouseAction(int key, int action) override;


#ifdef EKDEBUG
    void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;
#endif

  private:
    glm::vec3 mRayStart;
    glm::vec3 mRayDirection;
    float mRayRange = 0.0f;
    Physics::CastHitResult mLastCastHitResult;
  };

} // namespace Eklavya

#endif
