////
////  SceneDebugger.hpp
////  EklavyaEngine
////
////  Created by Ankit Singh Kushwah on 31/03/2024.
////
//
//#ifndef SceneDebugger_hpp
//#define SceneDebugger_hpp
//
//#include "EkActor.h"
//#include "UserInputListener.h"
//#include "imgui/imgui.h"
//namespace Eklavya::Renderer
//{
//  class DebugRenderer;
//}
//
//namespace Eklavya
//{
//  class EkScene;
//
//  enum ESceneDebugFlags
//  {
//    ALL =  0,
//    RENDERABLE = 1,
//    EKBODY = 2,
//    ANIMATED = 3,
//    MAX
//  };
//
//  struct SceneDebugger : UserInputListener
//  {
//    std::string GetDebugFlagString(ESceneDebugFlags flag);
//    bool CanTraverse(EkActor*& actor);
//    void TraverseToDebugDraw(EkActor* actor);
//    SceneDebugger(EkScene &scene);
//    void     DebugDraw(Renderer::DebugRenderer &debugRenderer);
//    void     OnKeyAction(int key, int action);
//    void     ImGuiProc();
//    void     Traverse(EkActor* &actor);
//    void     DebugTransform(EkActor* &actor);
//    void     DebugPhysics(EkActor* &actor);
//    void     DebugRenderComponent(EkActor* &actor);
//    void     DebugAnimationComponent(EkActor* &actor);
//    bool     mDebugScene = false;
//    bool mDebugPhysics = true;
//    EkScene &mScene;
//
//    std::array<bool,ESceneDebugFlags::MAX> mFilterFlags;
//    glm::vec4 mColliderColor;
//    glm::vec4 mBoundColor;
//
//    bool mMenuBarActive = false;
//    ImVec2 mTreeWindowSize;
//  };
//} // namespace Eklavya
//
//#endif /* SceneDebugger_hpp */
