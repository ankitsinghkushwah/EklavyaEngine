#include "EkScene.h"
#include "RenderComponent.hpp"
#include "Director.hpp"
#include "GLFWGame.h"
#include "GLRenderer.h"
#include "PhysicsWorld.h"
#include <CoreUtils/EkUtils.hpp>
#include "Collider.h"
#include "AnimationComponent.h"
#include "DebugRenderer.hpp"
#include "Cameras/FreeLookCamera.h"

using namespace Eklavya::Physics;
using namespace Eklavya::Renderer;

namespace Eklavya
{
  EkScene::EkScene(Director *director)
      : UserInputListener()
#ifdef EKDEBUG
        ,
        mSceneDebugger(*this)
#endif
  {
    mDirector = director;
    GLWindowContext &context = *director->GetCurrentContext();

    CameraParams     camParams{45.0f, context.GetAspectRatio(), 0.1f, 10000.0f};
    mDefaultCameraParams = camParams;
    mFreeLookCamera = std::make_shared<FreeLookCamera>(camParams);
    mCameraStack.push_back(mFreeLookCamera);

    mRenderer = std::make_unique<GLRenderer>(context);
    mRenderer->Init();
    mPhysicsWorld = std::make_unique<Physics::World>();
    sf::Listener::setGlobalVolume(50.f);
  }

  EkScene::~EkScene() {}

  void EkScene::FixedTick(float fixedDeltaTime)
  {
    for (SHARED_ACTOR &actor : mRootActors)
      {
        TraverseToFixedUpdateComponents(actor, fixedDeltaTime);
      }

    if (mSceneDebugger.mDebug == false)
      {
        mPhysicsWorld->Step(fixedDeltaTime);
      }
  }

  void EkScene::Tick(float dt)
  {
    for (SHARED_ACTOR actor : mRootActors)
      {
        TraverseToUpdate(actor, dt);
      }

    if (mSceneDebugger.mDebug == false)
      {
        CurrentCamera()->Update(dt);
        glm::vec3 camPos = CurrentCamera()->Position();
        sf::Listener::setPosition(camPos.x, camPos.y, camPos.z);
      }
  }

  void EkScene::TraverseToFixedUpdateComponents(SHARED_ACTOR parent,
                                                float        fixedDeltaTime)
  {
    for (SHARED_COMPONENT &component : parent->Components())
      {
        component->FixedTick(fixedDeltaTime);
      }

    for (SHARED_ACTOR kid : parent->Kids())
      {
        TraverseToFixedUpdateComponents(kid, fixedDeltaTime);
      }
  }

  void EkScene::TraverseToUpdate(SHARED_ACTOR actor, float dt)
  {
    if (actor->IsEnabled() == false)
      {
        return;
      }

    for (SHARED_COMPONENT &component : actor->Components())
      {
        component->Tick(dt);
      }

    if (SHARED_RENDERCOMPONENT renderComponent =
            actor->GetComponent<RenderComponent>("RenderComponent"))
      {
        renderComponent->mWorldMatrix = actor->Transform()->GetWorldMatrix();
        const Bound &bound = renderComponent->mBound;
#ifndef EKDEBUG
        renderComponent->mInsideFrustum = CurrentCamera()->GetFrustum().Test(
            renderComponent->mWorldMatrix, bound);
#endif
        if (renderComponent->mHasBones)
          {
            renderComponent->boneTransforms =
                mAnimators[renderComponent->mModelID]->GetPoseTransforms();
          }
        mRenderer->AddActor(renderComponent);
      }

    for (SHARED_ACTOR kid : actor->Kids())
      {
        kid->Transform()->ApplyParentTransform(
            actor->Transform()->GetWorldMatrix());

        TraverseToUpdate(kid, dt);
      }
  }

  void EkScene::AddActor(SHARED_ACTOR actor)
  {
    mRootActors.push_back(actor);

    if (SHARED_EKBODY body = actor->GetComponent<Physics::EkBody>("EkBody"))
      {
        mPhysicsWorld->AddBody(body.get());
      }

    if (SHARED_ANIMATOR animator =
            actor->GetComponent<Animator>("AnimationComponent"))
      {
        mAnimators[animator->GetModelID()] = animator;
      }
  }

  void EkScene::Draw() { mRenderer->Render(*this); }

  // Inputs

  void EkScene::OnKeyAction(int key, int action)
  {
    if (key == GLFW_KEY_PAGE_DOWN && action == GLFW_PRESS)
      {
        mCurrentCameraIdx = (mCurrentCameraIdx + 1) % mCameraStack.size();
      }
  }

#ifdef EKDEBUG
  void EkScene::ImGuiProc() { mSceneDebugger.ImGuiProc(); }

  void EkScene::DebugDraw(Renderer::DebugRenderer &debugRenderer)
  {
    mPhysicsWorld->OnDebugDraw(debugRenderer);
    mSceneDebugger.DebugDraw(debugRenderer);
    CurrentCamera()->DebugDraw(debugRenderer);
  }
#endif

} // namespace Eklavya
