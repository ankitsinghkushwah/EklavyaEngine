#include <Scene/EkScene.h>
#include <Components/RenderComponent.hpp>
#include <Director.hpp>
#include <GLFWGame.h>
#include <Renderer/GLRenderer.h>
#include <EkPhysics/PhysicsWorld.h>
#include <CoreUtils/EkUtils.hpp>
#include <EkPhysics/Collider.h>
#include <Components/AnimationComponent.h>
#include <Renderer/DebugRenderer.hpp>
#include "Cameras/FreeLookCamera.h"
#include "Components/AnimationIKSolverComponent.hpp"

using namespace Eklavya::Physics;
using namespace Eklavya::Renderer;

namespace Eklavya
{
	EkScene::EkScene(Director &director)
		:
		UserInputListener()
		, mDirector(director)
		, mRenderer(*director.GetCurrentContext())
#ifdef EKDEBUG
		, mSceneDebugger(*this)
#endif
	{
		GLWindowContext &context = *director.GetCurrentContext();

		CameraParams camParams{45.0f, context.GetAspectRatio(), 0.1f, 10000.0f};
		mDefaultCameraParams = camParams;
		mFreeLookCamera = std::make_shared<FreeLookCamera>(camParams);
		mCameraStack.push_back(mFreeLookCamera);


		sf::Listener::setGlobalVolume(50.f);

		mRenderer.Init();
	}

	EkScene::~EkScene() {}

	void EkScene::FixedTick(float fixedDeltaTime)
	{
#ifdef EKDEBUG
		if (mSceneDebugger.mDebugPhysics == false)
		{
			for (const UniqueActor &actor: mRootActors)
			{
				TraverseToFixedUpdateComponents(actor, fixedDeltaTime);
			}
			mPhysicsWorld.Step(fixedDeltaTime);
		}
#else
		mPhysicsWorld.Step(fixedDeltaTime);
		for (const UniqueActor &actor: mRootActors)
		{
			TraverseToFixedUpdateComponents(actor, fixedDeltaTime);
		}
#endif
	}

	void EkScene::Tick(float dt)
	{
		for (UniqueActor &actor: mRootActors)
		{
			TraverseToUpdate(actor, dt);
		}
#ifdef EKDEBUG
		if (mSceneDebugger.mDebugScene == false)
		{
			CurrentCamera()->Update(dt);
		}
#else
		CurrentCamera()->Update(dt);
#endif
		glm::vec3 camPos = CurrentCamera()->Position();
		sf::Listener::setPosition(camPos.x, camPos.y, camPos.z);
	}

	void EkScene::TraverseToFixedUpdateComponents(const UniqueActor &parent, float fixedDeltaTime)
	{
		for (const std::unique_ptr<EkComponent> &component: parent->Components())
		{
			component->FixedTick(fixedDeltaTime);
		}

		for (const UniqueActor &kid: parent->Kids())
		{
			TraverseToFixedUpdateComponents(kid, fixedDeltaTime);
		}
	}

	void EkScene::TraverseToUpdate(const UniqueActor &actor, float dt)
	{
		if (actor->IsEnabled() == false)
		{
			return;
		}

		for (const std::unique_ptr<EkComponent> &component: actor->Components())
		{
			component->Tick(dt);
		}

		if (RenderComponent *renderComponent = actor->GetComponent<RenderComponent>(
			CoreComponentIds::RENDER_COMPONENT_ID))
		{
			renderComponent->mWorldMatrix = actor->Transform().GetWorldMatrix();
			const Bound &bound = renderComponent->mBound;
			renderComponent->mInsideFrustum = CurrentCamera()->GetFrustum().Test(renderComponent->mWorldMatrix, bound);

			renderComponent->mHasBones = mAnimators.find(renderComponent->mModelID) != mAnimators.end();
			if (renderComponent->mHasBones)
			{
				renderComponent->boneTransforms = mAnimators[renderComponent->mModelID]->GetPoseTransforms();
			}

			if (renderComponent->mHasBones == false)
			{
				renderComponent->mHasBones = mIKSolvers.find(renderComponent->mModelID) != mIKSolvers.end();
				if (renderComponent->mHasBones)
				{
					renderComponent->boneTransforms = mIKSolvers[renderComponent->mModelID]->GetPoseTransforms();
				}
			}

			mRenderer.AddActor(*renderComponent);
		}

		for (const UniqueActor &kid: actor->Kids())
		{
			kid->Transform().ApplyParentTransform(actor->Transform().GetWorldMatrix());

			TraverseToUpdate(kid, dt);
		}
	}

	void EkScene::AddActor(UniqueActor &newActor)
	{
		mRootActors.push_back(std::move(newActor));
		UniqueActor &actor = mRootActors.back();

		if (EkBody *body = actor->GetComponent<Physics::EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID))
		{
			mPhysicsWorld.AddBody(body);
		}

		if (AnimationComponent *animator = actor->GetComponent<AnimationComponent>(
			CoreComponentIds::ANIMATION_COMPONENT_ID))
		{
			mAnimators[animator->GetModelID()] = animator;
		}
		if (AnimationIKSolver *solver = actor->GetComponent<AnimationIKSolver>(
			CoreComponentIds::IK_SOLVER_COMPONENT))
		{
			mIKSolvers[solver->mModelID] = solver;
		}
	}

	void EkScene::Draw()
	{
		mRenderer.Render(*this);
	}

	void EkScene::Cleanup()
	{
		DebugRenderer::GetInstance().ClearAddedShapes();
	}

	void EkScene::DeleteDeferredDeadActors()
	{
		for (EkActorID deadActorId: mActorsToBeRemoved)
		{
			auto iter = std::find_if(mRootActors.begin(), mRootActors.end(), [deadActorId](const UniqueActor &actor)
			{
				return actor->ID() == deadActorId;
			});

			if (iter != mRootActors.end())
			{
				UniqueActor &actor = *iter;
				if (EkBody *body = actor->GetComponent<Physics::EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID))
				{
					mPhysicsWorld.RemoveBody(body);
				}

				if (AnimationComponent *animator = actor->GetComponent<AnimationComponent>(
					CoreComponentIds::ANIMATION_COMPONENT_ID))
				{
					mAnimators.erase(animator->GetModelID());
				}
				iter->reset();
				mRootActors.erase(iter);
			}
		}
	}

	// Inputs

	void EkScene::OnKeyAction(int key, int action)
	{
		if (key == GLFW_KEY_C && action == GLFW_PRESS)
		{
			mCurrentCameraIdx = (mCurrentCameraIdx + 1) % mCameraStack.size();
		}
	}

	void EkScene::ImGuiProc()
	{
#ifdef EKDEBUG
		mSceneDebugger.ImGuiProc();
		mPhysicsWorld.ImGuiProc();
#endif
	}


	void EkScene::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
#ifdef EKDEBUG
		mSceneDebugger.DebugDraw(debugRenderer);
#endif
		CurrentCamera()->DebugDraw(debugRenderer);
	}
} // namespace Eklavya
