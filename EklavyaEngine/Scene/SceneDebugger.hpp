//
//  SceneDebugger.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 31/03/2024.
//

#ifndef SceneDebugger_hpp
#define SceneDebugger_hpp

#include "EkActor.h"
#include "UserInputListener.h"
#include "imgui/imgui.h"
namespace Eklavya::Renderer
{
  class DebugRenderer;
}

namespace Eklavya
{
  class EkScene;
  
  enum ESceneDebugFlags
  {
    ALL =  0,
    RENDERABLE = 1,
    EKBODY = 2,
    ANIMATED = 3,
    MAX
  };
  
  struct SceneDebugger : UserInputListener
  {
    std::string GetDebugFlagString(ESceneDebugFlags flag);
    bool CanTraverse(SHARED_ACTOR& actor);
    void TraverseToDebugDraw(SHARED_ACTOR actor);
    SceneDebugger(EkScene &scene);
    void     DebugDraw(Renderer::DebugRenderer &debugRenderer);
    void     OnKeyAction(int key, int action);
    void     ImGuiProc();
    void     Traverse(SHARED_ACTOR &actor);
    void     DebugTransform(SHARED_ACTOR &actor);
    void     DebugPhysics(SHARED_ACTOR &actor);
    void     DebugRenderComponent(SHARED_ACTOR &actor);
    void     DebugAnimationComponent(SHARED_ACTOR &actor);
    bool     mDebug = false;
    EkScene &mScene;
    
    std::array<bool,ESceneDebugFlags::MAX> mFilterFlags;
    glm::vec4 mColliderColor;
    glm::vec4 mBoundColor;
    
    bool mMenuBarActive = false;
    ImVec2 mTreeWindowSize;
  };
} // namespace Eklavya

#endif /* SceneDebugger_hpp */
