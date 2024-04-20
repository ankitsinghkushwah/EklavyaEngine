#ifndef INC_EKSCENE
#define INC_EKSCENE

#include <string>
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include "Cameras/ICamera.hpp"
#include "EkActor.h"
#include "UserInputListener.h"
#include <SFML/Audio.hpp>
#include "AnimationData.h"
#include "AnimationComponent.h"

#ifdef EKDEBUG
#include "SceneDebugger.hpp"
#endif

namespace Eklavya
{
  // forward declarations

  class Animator;
  class FreeLookCamera;

  namespace Renderer
  {
    class GLRenderer;
    class DebugRenderer;
  } // namespace Renderer

  namespace Physics
  {
    class World;
  }

  class Director;

  // end

  class EkScene : public UserInputListener
  {
#ifdef EKDEBUG
      friend class SceneDebugger;
#endif
      
  public:
    EkScene(Director *director);

    virtual void FixedTick(float fixedDeltaTime) final;
    virtual void Tick(float deltaTime) final;

    
    virtual ~EkScene();
    void                  AddActor(SHARED_ACTOR actor);

    const Physics::World &GetPhysics() const { return *mPhysicsWorld.get(); }

    CameraParams          DefaulCameraParams() const { return mDefaultCameraParams; }

    void                  OverrideCamera(std::shared_ptr<ICamera> cameraImpl)
    {
      mCurrentCameraIdx = static_cast<int>(mCameraStack.size());
      mCameraStack.push_back(cameraImpl);
    }

    const std::shared_ptr<ICamera> &CurrentCamera() const
    {
      return mCameraStack[mCurrentCameraIdx];
    }

    void Draw();

    // UserInputListener overrides
    void OnMouseAction(int key, int action) override {}

    void OnKeyAction(int key, int action) override;

    void OnCursorMove(double x, double y) override {}

  protected:
    void TraverseToUpdate(SHARED_ACTOR actor, float deltaTime);
    void TraverseToFixedUpdateComponents(SHARED_ACTOR actor, float fixedDeltaTime);

    std::shared_ptr<FreeLookCamera>              mFreeLookCamera;
    std::vector<SHARED_ACTOR>                    mRootActors;
    std::unique_ptr<Renderer::GLRenderer>        mRenderer;
    std::unique_ptr<Physics::World>              mPhysicsWorld;
    Director                                    *mDirector;
    std::unordered_map<ModelID, SHARED_ANIMATOR> mAnimators;

    std::vector<std::shared_ptr<ICamera>>        mCameraStack;
    int                                          mCurrentCameraIdx = 0;

    CameraParams                                  mDefaultCameraParams;

#ifdef EKDEBUG
  public:
    virtual void ImGuiProc();
    virtual void DebugDraw(Renderer::DebugRenderer &debugRenderer);
  private:
      SceneDebugger mSceneDebugger;
#endif
  };
} // namespace Eklavya

#endif
