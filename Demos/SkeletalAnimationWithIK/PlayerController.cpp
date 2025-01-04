//
//  State.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/04/2024.
//

#include "PlayerController.hpp"
#include "Scene/EkScene.h"
#include "Renderer/DebugRenderer.hpp"
#include "imgui/imgui.h"
#include "glfw/glfw3.h"
#include "AssetManager/AssetManager.h"
#include "EkPhysics/PhysicsWorld.h"

using namespace Eklavya;
using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

PlayerController::PlayerController(EkActor& owner, Eklavya::EkScene& scene)
    : UserInputListener()
    , EkComponent(owner, ThirdPersonDemoComponentIds::PLAYER_CONTROLLER_ID)
    , mScene(scene)
    , mStandingMovementColliderSize(40.0f, 100, 40.0f)
    , mCurrGravityDir(0.0f, 1.0f, 0.0f)
{
	mLastDirection = MD_IDLE;
}

std::string PlayerController::GetDirectionString(EPlayerStance stance, EMovementDirection direction)
{
	if (stance == CROUCH_MOVEMENT && direction == EMovementDirection::MD_BACKWARD)
	{
		return "jog_forward";
	}
	std::string animationName = "";

	switch (direction)
	{
		case EMovementDirection::MD_IDLE:
			animationName = "idle";
			break;
		case EMovementDirection::MD_FORWARD:
			animationName = "jog_forward";
			break;
		case EMovementDirection::MD_BACKWARD:
			animationName = "jog_backward";
			break;
		case EMovementDirection::MD_TURN_LEFT:
			animationName = "turn_left";
			break;
		case EMovementDirection::MD_TURN_RIGHT:
			animationName = "turn_right";
			break;
	}

	return animationName;
}

void PlayerController::LoadAnimations()
{
	// Standing Movement
	for (int movement_type = 0; movement_type < PS_MAX; ++movement_type)
	{
		std::string prefix = "standing_";
		if (movement_type == 1)
		{
			prefix = "crouch_";
		}

		for (int direction = 0; direction < EMovementDirection::MD_MAX; direction++)
		{
			std::string directionName = "";

			directionName = GetDirectionString((EPlayerStance)movement_type, (EMovementDirection)direction);

			std::string animationName = "characters/swat/animations/" + prefix + directionName;

			SHARED_ANIMATION anim = AssetManager::GetInstance().GetAsset<Animation>(animationName);
			assert(anim);
			mStandingMovementAnims[movement_type][direction] = anim;
		}
	}
}

void PlayerController::Init()
{
	LoadAnimations();
	mAnimator = GetOwner().GetComponent<AnimationComponent>(CoreComponentIds::ANIMATION_COMPONENT_ID);
	mPosition = GetOwner().Transform().Position();
	mRotationAngle = glm::eulerAngles(GetOwner().Transform().Rotation()).y;
	//mRotationTime = mStandingMovementAnims[0][3]->GetRealDuration();
	SetStance(EPlayerStance::STANDING_MOVEMENT);
}

void PlayerController::SetStance(EPlayerStance stance)
{
	mStance = stance;

	mAnimator->PlayAnimation(mStandingMovementAnims[mStance][mDirection], true);
}

void PlayerController::OnKeyAction(int key, int action)
{
	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_PRESS)
	{
		SetStance(EPlayerStance::CROUCH_MOVEMENT);
	}

	if (key == GLFW_KEY_LEFT_SHIFT && action == GLFW_RELEASE)
	{
		SetStance(EPlayerStance::STANDING_MOVEMENT);
	}

	if (mTurning == false)
	{
		if (key == GLFW_KEY_LEFT && action == GLFW_PRESS)
		{
			mTurning = true;
			mShouldTurnRight = false;
			mDirection = MD_TURN_LEFT;
		}

		if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
		{
			mTurning = true;
			mShouldTurnRight = true;
			mDirection = MD_TURN_RIGHT;
		}

		if (mTurning)
		{
			mRotationTimeElapsed = 0.0f;
			mAnimator->PlayAnimation(mStandingMovementAnims[mStance][mDirection]);
			mRotationTime = mStandingMovementAnims[mStance][mDirection]->GetRealDuration();
		}
	}
}

void PlayerController::Tick(float dt)
{
	glm::vec3 rayPos = GetOwner().Transform().Position();

	mCastResult = mScene.GetPhysics().RayCast(rayPos, glm::vec3(0.0f, -1.0f, 0.0f), mCastRange, 1);
	if (mCastResult.success)
	{
		mCurrGravityDir = mCastResult.body->GetOwner().Transform().up();
		mPosition.y = mCastResult.position.y;
		GetOwner().Transform().SetPosition(mPosition);
	}
	else
		mPosition += mCurrGravityDir * mGravity * dt;

	if (mTurning == false)
	{
		StanceUpdate(dt);
	}
	else
	{
		TurnUpdate(dt);
	}
}

void PlayerController::StanceUpdate(float dt)
{
	if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_UP))
	{
		mDirection = EMovementDirection::MD_FORWARD;
		mPosition += GetOwner().Transform().forward() * dt * mSpeed;
		GetOwner().Transform().SetPosition(mPosition);
	}
	else if (mStance == EPlayerStance::STANDING_MOVEMENT && InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_DOWN))
	{
		mDirection = EMovementDirection::MD_BACKWARD;
		mPosition += GetOwner().Transform().forward() * dt * -mSpeed;
		GetOwner().Transform().SetPosition(mPosition);
	}
	else
	{
		mDirection = EMovementDirection::MD_IDLE;
	}

	if (mLastDirection != mDirection)
	{
		SHARED_ANIMATION animation = mStandingMovementAnims[mStance][mDirection];
		mAnimator->PlayAnimation(animation, true);
		mLastDirection = mDirection;
	}
}

void PlayerController::TurnUpdate(float dt)
{
	mRotationTimeElapsed += dt;
	if (mRotationTimeElapsed >= mRotationTime)
	{
		mRotationAngle = mShouldTurnRight ? mRotationAngle - 90.0f : mRotationAngle + 90.0f;
		GetOwner().Transform().SetRotation(glm::vec3(0.0f, mRotationAngle, 0.0f));
		mTurning = false;
		mLastDirection = mDirection;
	}
}

#ifdef EKDEBUG

void PlayerController::ImGuiProc()
{
}

void PlayerController::DebugDraw(Eklavya::Renderer::DebugRenderer& debugRenderer)
{
	if (mCastResult.success)
	{
		debugRenderer.DrawLine(GetOwner().Transform().Position(), mCastResult.position, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), 5.0f);
		debugRenderer.DrawSphere(mCastResult.position, 2.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else
	{
		debugRenderer.DrawLine(GetOwner().Transform().Position(),
		                       GetOwner().Transform().Position() + glm::vec3(0.0f, -mCastRange, 0.0f),
		                       glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		                       5.0f);
	}
	mCastResult.success = false;
}
#endif
