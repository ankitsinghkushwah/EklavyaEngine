////
////  SceneDebugger.cpp
////  EklavyaEngine
////
////  Created by Ankit Singh Kushwah on 31/03/2024.
////
//
//#include "SceneDebugger.hpp"
//#include "DebugRenderer.hpp"
//#include "EkScene.h"
//#include "RenderComponent.hpp"
//#include "Director.hpp"
//#include "GLFWGame.h"
//#include "GLRenderer.h"
//#include "PhysicsWorld.h"
//#include <CoreUtils/EkUtils.hpp>
//#include "Collider.h"
//#include "AnimationComponent.h"
//#include "DebugRenderer.hpp"
//#include "Cameras/FreeLookCamera.h"
//#include "imgui/imgui.h"
//
//using namespace Eklavya::Physics;
//using namespace Eklavya::Renderer;
//
//namespace Eklavya
//{
//  SceneDebugger::SceneDebugger(EkScene &scene)
//      : UserInputListener(), mScene(scene)
//  {
//    for (int i = 0; i < ESceneDebugFlags::MAX; i++)
//      {
//        mFilterFlags[i] = i == 0 ? false : true;
//      }
//
//    mColliderColor = glm::vec4(0.2f, 0.6f, 0.0f, 0.3f);
//    mBoundColor = glm::vec4(0.8f, 0.2f, 0.4f, 0.3f);
//  }
//
//  void SceneDebugger::TraverseToDebugDraw(EkActor* actor)
//  {
////    if (actor->mDebugDrawComponents)
////      {
////        for (auto &component : actor->Components())
////          {
////            component->DebugDraw(mScene.mRenderer->GetDebugRenderer());
////          }
////      }
////    EkBody* body = actor->GetComponent<Physics::EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
////    if (body && body->mShowCollider)
////      {
////        glm::vec3 oiler = glm::eulerAngles(actor->Transform().Rotation());
////
////        if (body->GetCollider()->GetType() == Physics::EColliderType::BOX)
////          {
////            glm::vec3 extents = std::dynamic_pointer_cast<Physics::BoxColliderComponent>(
////                                    body->GetCollider())
////                                    ->GetHalfSize() *
////                                2.0f;
////
////            mScene.mRenderer->GetDebugRenderer().DrawBox(
////                actor->Transform().Position() +
////                    body->GetCollider()->GetOffset(),
////                oiler, extents, mColliderColor);
////          }
////
////        if (body->GetCollider()->GetType() == Physics::EColliderType::SPHERE)
////          {
////            float radius = std::dynamic_pointer_cast<Physics::SphereColliderComponent>(
////                               body->GetCollider())
////                               ->GetRadius();
////
////            mScene.mRenderer->GetDebugRenderer().DrawSphere(
////                actor->Transform().Position(), radius, mColliderColor);
////          }
////      }
////
////    SHARED_RENDERCOMPONENT renderComponent =
////        actor->GetComponent<RenderComponent>("RenderComponent");
////    if (renderComponent && renderComponent->mShowBound)
////      {
////        mScene.mRenderer->GetDebugRenderer().DrawBound(
////            actor->Transform().GetWorldMatrix(), renderComponent->mBound,
////            mBoundColor);
////      }
////
////    for (SHARED_ACTOR kid : actor->Kids())
////      TraverseToDebugDraw(kid);
//  }
//
//  void SceneDebugger::DebugDraw(Renderer::DebugRenderer &debugRenderer)
//  {
//    for (auto &actor : mScene.mRootActors)
//      {
//        TraverseToDebugDraw(actor);
//      }
//  }
//
//  void SceneDebugger::OnKeyAction(int key, int action)
//  {
//    if (key == GLFW_KEY_P && action == GLFW_PRESS)
//      {
//        mDebugScene = !mDebugScene;
//        mScene.mDirector->HideMouse(mDebugScene == false);
//        mScene.mFreeLookCamera->SetEnabled(mDebugScene == false);
//      }
//
//      if(key == GLFW_KEY_O && action == GLFW_PRESS)
//      {
//        mDebugPhysics = !mDebugPhysics;
//      }
//
//      if(mDebugPhysics && key == GLFW_KEY_RIGHT)
//      {
//        mScene.mPhysicsWorld->Step(1/30.0f);
//      }
//  }
//
//  bool SceneDebugger::CanTraverse(SHARED_ACTOR &actor)
//  {
//    if (mFilterFlags[ESceneDebugFlags::RENDERABLE] &&
//        actor->GetComponent<RenderComponent>("RenderComponent") != nullptr)
//      {
//        return true;
//      }
//    if (mFilterFlags[ESceneDebugFlags::EKBODY] &&
//        actor->GetComponent<EkBody>("EkBody") != nullptr)
//      {
//        return true;
//      }
//    if (mFilterFlags[ESceneDebugFlags::ANIMATED] &&
//        actor->GetComponent<AnimationComponent>("AnimationComponent") != nullptr)
//      {
//        return true;
//      }
//    return false;
//  }
//
//  std::string SceneDebugger::GetDebugFlagString(ESceneDebugFlags flag)
//  {
//    switch (flag)
//      {
//      case ALL:
//        return "all";
//        break;
//      case ESceneDebugFlags::RENDERABLE:
//        return "Renderable";
//        break;
//      case ESceneDebugFlags::EKBODY:
//        return "EkBody";
//        break;
//
//      case ESceneDebugFlags::ANIMATED:
//        return "Animated";
//        break;
//      }
//
//    return "error";
//  }
//
//  void SceneDebugger::ImGuiProc()
//  {
//    if (mDebugScene == false)
//      return;
//
//    ImGui::Begin("SceneDebugger");
//
//    for (int i = 0; i < ESceneDebugFlags::MAX; i++)
//      {
//        bool        enabled = mFilterFlags[i];
//        std::string flagName = GetDebugFlagString((ESceneDebugFlags)i);
//        if (ImGui::Checkbox(flagName.c_str(), &enabled))
//          {
//            mFilterFlags[i] = enabled;
//          }
//      }
//
//    // Scene Tree
//    if (ImGui::TreeNode("SceneRoot"))
//      {
//        for (auto &rootActor : mScene.mRootActors)
//          {
//            ImGui::PushID(std::to_string(rootActor->ID()).c_str());
//            Traverse(rootActor);
//            ImGui::PopID();
//          }
//        ImGui::TreePop();
//      }
//
//    ImGui::End();
//  }
//
//  void SceneDebugger::Traverse(SHARED_ACTOR &actor)
//  {
//    bool canTraverse = CanTraverse(actor);
//
//    if (canTraverse)
//      {
//        if (ImGui::TreeNode(actor->Name().c_str()))
//          {
//            if (CanTraverse(actor))
//              {
//                // Transform
//                DebugTransform(actor);
//
//                // Rendering
//                DebugRenderComponent(actor);
//
//                // Rigidbody
//                DebugPhysics(actor);
//
//                // Animation
//                DebugAnimationComponent(actor);
//              }
//
//            for (auto &kid : actor->Kids())
//              {
//                ImGui::PushID(std::to_string(kid->ID()).c_str());
//                Traverse(kid);
//                ImGui::PopID();
//              }
//
//            ImGui::TreePop();
//          }
//      }
//    else
//      {
//        for (auto &kid : actor->Kids())
//          {
//            ImGui::PushID(std::to_string(kid->ID()).c_str());
//            Traverse(kid);
//            ImGui::PopID();
//          }
//      }
//  }
//
//  void SceneDebugger::DebugTransform(SHARED_ACTOR &actor)
//  {
//
//    if (ImGui::TreeNode(("General")))
//      {
//        SHARED_EKBODY    ekBody = actor->GetComponent<EkBody>("EkBody");
//        SHARED_TRANSFORM transform = actor->Transform();
//        glm::vec3        position = transform->Position();
//        glm::vec3        eulers = glm::eulerAngles(transform->Rotation());
//
//        if (ImGui::InputFloat3("Position", &position[0]))
//          {
//
//            transform->SetPosition(position);
//            if (ekBody)
//              ekBody->SetPos(position);
//          }
//
//        if (ImGui::InputFloat3("Rotation", &eulers[0]))
//          {
//            transform->SetRotation(eulers);
//            if (ekBody)
//              ekBody->SetRotation(eulers);
//          }
//
//        bool IsEnabled = actor->IsEnabled();
//
//        if (ImGui::Checkbox("Enabled", &IsEnabled))
//          {
//            actor->SetEnabled(IsEnabled);
//          }
//
//        bool canDebugDrawComponents = actor->mDebugDrawComponents;
//
//        if (ImGui::Checkbox("Debug Draw Components", &canDebugDrawComponents))
//          {
//            actor->mDebugDrawComponents = canDebugDrawComponents;
//          }
//
//        ImGui::TreePop();
//      }
//  }
//
//  void SceneDebugger::DebugPhysics(SHARED_ACTOR &actor)
//  {
//
//    if (ImGui::TreeNode(("Rigidbody")))
//      {
//        if (SHARED_EKBODY body = actor->GetComponent<EkBody>("EkBody"))
//          {
//            bool showCollider = body->mShowCollider;
//            if (ImGui::Checkbox("Show Collider", &showCollider))
//              {
//                body->mShowCollider = showCollider;
//              }
//          }
//        ImGui::TreePop();
//      }
//  }
//
//  void SceneDebugger::DebugRenderComponent(SHARED_ACTOR &actor)
//  {
//    if (ImGui::TreeNode(("Rendering")))
//      {
//        if (SHARED_RENDERCOMPONENT renderComponent =
//                actor->GetComponent<RenderComponent>("RenderComponent"))
//          {
//            bool projectShadow = renderComponent->mProjectsShadow;
//            if (ImGui::Checkbox("cast shadow", &projectShadow))
//              {
//                renderComponent->mProjectsShadow = projectShadow;
//              }
//
//            bool animated = renderComponent->mHasBones;
//            if (ImGui::Checkbox("animated", &animated))
//              {
//                renderComponent->mHasBones = animated;
//              }
//
//            bool showBounds = renderComponent->mShowBound;
//            if (ImGui::Checkbox("Show Bounds", &showBounds))
//              {
//                renderComponent->mShowBound = showBounds;
//              }
//          }
//        ImGui::TreePop();
//      }
//  }
//
//  void SceneDebugger::DebugAnimationComponent(SHARED_ACTOR &actor) {}
//
//} // namespace Eklavya
