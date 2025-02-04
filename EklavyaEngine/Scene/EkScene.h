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

	class AnimationComponent;
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

	class EkScene : public UserInputListener
	{
#ifdef EKDEBUG
		friend class SceneDebugger;
#endif
		friend class Director;

	  public:
		EkScene(Director* director);

		virtual void FixedTick(float fixedDeltaTime) final;
		virtual void Tick(float deltaTime);

		virtual ~EkScene();

		void AddActor(UniqueActor& actor);
		void RemoveActor(EkActorID id)
		{
			mActorsToBeRemoved.push_back(id);
		}

		const Physics::World& GetPhysics() const
		{
			return *mPhysicsWorld.get();
		}
  
    const Renderer::GLRenderer& GetRenderer() const
    {
      return *mRenderer.get();
    }

		CameraParams DefaulCameraParams() const
		{
			return mDefaultCameraParams;
		}

		void OverrideCamera(std::shared_ptr<ICamera> cameraImpl)
		{
			mCurrentCameraIdx = static_cast<int>(mCameraStack.size());
			mCameraStack.push_back(cameraImpl);
		}

		const std::shared_ptr<ICamera>& CurrentCamera() const
		{
			return mCameraStack[mCurrentCameraIdx];
		}

		void Draw();

		// UserInputListener overrides
		void OnMouseAction(int key, int action) override
		{
		}

		void OnKeyAction(int key, int action) override;

		void OnCursorMove(double x, double y) override
		{
		}

	  protected:
		void TraverseToUpdate(const UniqueActor& actor, float deltaTime);
		void TraverseToFixedUpdateComponents(const UniqueActor& actor, float fixedDeltaTime);

		CameraParams mDefaultCameraParams;

#ifdef EKDEBUG
	  public:
		virtual void ImGuiProc();
		virtual void DebugDraw(Renderer::DebugRenderer& debugRenderer);

	  protected:
		SceneDebugger mSceneDebugger;
#endif

	  private:
		std::shared_ptr<FreeLookCamera>                  mFreeLookCamera;
		std::vector<UniqueActor>                         mRootActors;
		std::vector<EkActorID>                           mActorsToBeRemoved;
		std::unique_ptr<Renderer::GLRenderer>            mRenderer;
		std::unique_ptr<Physics::World>                  mPhysicsWorld;
		Director*                                        mDirector;
		std::unordered_map<ModelID, AnimationComponent*> mAnimators;

		std::vector<std::shared_ptr<ICamera>> mCameraStack;
		int                                   mCurrentCameraIdx = 0;
  
    void Cleanup();
	};
} // namespace Eklavya

#endif
