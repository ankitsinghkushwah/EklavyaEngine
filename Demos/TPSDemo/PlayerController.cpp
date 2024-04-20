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

PlayerController::PlayerController(Eklavya::EkScene &scene)
    : UserInputListener(), EkComponent("PlayerController"), mScene(scene),
      mStandingMovementColliderSize(40.0f, 100, 40.0f)
{

  mLastPhase = 0;
  mLastDirection = MD_FORWARD;
}

std::string PlayerController::GetDirectionString(EMovementDirection direction)
{
  std::string animationName = "";

  switch (direction)
    {
    case EMovementDirection::MD_LEFT:
      animationName = "left";
      break;
    case EMovementDirection::MD_RIGHT:
      animationName = "right";
      break;
    case EMovementDirection::MD_FORWARD:
      animationName = "forward";
      break;
    case EMovementDirection::MD_BACKWARD:
      animationName = "back";
      break;

    case EMovementDirection::MD_MAX:
      animationName = "";
      break;
    }

  return animationName;
}

void PlayerController::LoadAnimations()
{

  // Standing Movement
  for (int phase = 0; phase < 3; ++phase)
    {
      std::string prefix = "idle";
      if (phase == 1)
        {
          prefix = "walk_";
        }
      else if (phase == 2)
        {
          prefix = "run_";
        }

      for (int direction = 0; direction < EMovementDirection::MD_MAX;
           direction++)
        {
          std::string directionName = "";

          directionName = GetDirectionString((EMovementDirection)direction);

          std::string animationName = "characters/player/standing_movement/" +
                                      prefix + (phase > 0 ? directionName : "");

          SHARED_ANIMATION anim =
              AssetManager::GetInstance().GetAsset<Animation>(animationName);
          assert(anim);
          mStandingMovementAnims[phase][direction] = anim;
        }
    }

  // Jumping

  mJumpPhaseAnims[0] = AssetManager::GetInstance().GetAsset<Animation>(
      "characters/player/jumping/takeoff");
  mJumpPhaseAnims[1] = AssetManager::GetInstance().GetAsset<Animation>(
      "characters/player/jumping/flight");
  mJumpPhaseAnims[2] = AssetManager::GetInstance().GetAsset<Animation>(
      "characters/player/jumping/land");
}

void PlayerController::Init()
{
  LoadAnimations();
  mAnimator = mOwner->GetComponent<Animator>("AnimationComponent");
  mBody = mOwner->GetComponent<EkBody>("EkBody");
  mBoxCollider = std::static_pointer_cast<BoxCollider>(mBody->GetCollider());

  mBoxCollider->SetHalfSize(mStandingMovementColliderSize);
  mBoxCollider->SetOffset(
      glm::vec3(0.0f, mStandingMovementColliderSize.y / 2.0f, 0.0f));

  mLastPosition = mOwner->Transform()->Position();

  SetStance(EPlayerStance::STANDING_MOVEMENT);
}

bool PlayerController::CanSwitchToStance(EPlayerStance stance)
{
  return mStance != stance;
}

void PlayerController::SetStance(EPlayerStance stance)
{
  if (CanSwitchToStance(stance) == false)
    {
      return;
    }
  mStance = stance;
  switch (mStance)
    {
    case EPlayerStance::STANDING_MOVEMENT:
      StandingMovementStart();
      break;
    case EPlayerStance::JUMPING:
      JumpingStart();
    default:
      break;
    }
}

void PlayerController::SetDirection(EMovementDirection direction) {}

EMovementDirection PlayerController::GetDirection(float axisX, float axisY)
{
  glm::vec2 dir(axisX, axisY);
  dir = glm::normalize(dir);

  glm::vec2 localForward(0.0f, -1.0f);
  glm::vec2 localRight(1.0f, 0.0f);

  float     rightDot = glm::dot(localRight, dir);
  float     forwarDot = glm::dot(localForward, dir);

  float     threshold = 0.3f;

  if (rightDot > threshold)
    return EMovementDirection::MD_RIGHT;
  else if (rightDot < -threshold)
    return EMovementDirection::MD_LEFT;
  else if (forwarDot > threshold)
    return EMovementDirection::MD_FORWARD;
  else if (forwarDot < -threshold)
    return EMovementDirection::MD_BACKWARD;

  return MD_FORWARD;
}

int PlayerController::GetPhase(float speed)
{
  if (speed > 0 && speed < .5f)
    return 0;

  else if (speed > .5f)
    return 2;

  return 0;
}

void PlayerController::OnKeyAction(int key, int action)
{
  if (key == GLFW_GAMEPAD_BUTTON_A && action == GLFW_PRESS)
    {
      SetStance(EPlayerStance::JUMPING);
    }
}

void PlayerController::UpdateRotation(float dt)
{
  if (InputHandler::GetInstance()->IsControllerConnected())
    {
      float axisZ =
          InputHandler::GetInstance()->GetAxis(GLFW_GAMEPAD_AXIS_RIGHT_X);

      mAngleOffset += mRotationSpeed * dt * -axisZ;
      glm::quat finalRot =
          glm::quat(glm::vec3(0.0f, glm::radians(mAngleOffset), 0.0f));
      mBody->SetOrientation(finalRot);
    }
}

void PlayerController::Tick(float dt)
{

  if (InputHandler::GetInstance()->IsControllerConnected())
    {
      int                  count = 0;
      const unsigned char *buttons =
          glfwGetJoystickButtons(GLFW_JOYSTICK_1, &count);
      if (buttons[0] == GLFW_PRESS)
        {
          SetStance(EPlayerStance::JUMPING);
        }
    }

  switch (mStance)
    {
    case EPlayerStance::STANDING_MOVEMENT:
      StandingMovementUpdate(dt);
      UpdateRotation(dt);
      break;
    case EPlayerStance::JUMPING:
      JumpingUpdate(dt);
      UpdateRotation(dt);
      break;

    default:
      break;
    }
}

// STANCES FUNCTIONS

// STANDING MOVEMENT
void PlayerController::StandingMovementStart() {}

void PlayerController::StandingMovementUpdate(float dt)
{

  if (InputHandler::GetInstance()->IsControllerConnected())
    {
      float axisX =
          InputHandler::GetInstance()->GetAxis(GLFW_GAMEPAD_AXIS_LEFT_X);

      float axisY =
          InputHandler::GetInstance()->GetAxis(GLFW_GAMEPAD_AXIS_LEFT_Y);

      mPosition += mOwner->Transform()->forward() * dt * -axisY * mSpeed;
      mPosition += mOwner->Transform()->right() * dt * -axisX * mSpeed;

      mBody->SetPos(mPosition);

      float speed = glm::length(mPosition - mLastPosition);
      mLastPosition = mPosition;
      int                phase = GetPhase(speed);
      EMovementDirection direction = GetDirection(axisX, axisY);
      std::string        directionName = GetDirectionString(direction);

      if (phase != mLastPhase || direction != mLastDirection)
        {
          SHARED_ANIMATION animation = mStandingMovementAnims[phase][direction];
          mAnimator->PlayAnimation(animation);
          mLastPhase = phase;
          mLastDirection = direction;
        }
    }
}

// JUMPING

void PlayerController::JumpingStart()
{
  mJumpPhase = 0;
  mJumpCastResult = CastHitResult();
  mJumpPhaseElapsed = .2f;
  mBody->SetVelocity(glm::vec3(0.0f));
  mAnimator->PlayAnimation(mJumpPhaseAnims[mJumpPhase + 1]);
}

void PlayerController::JumpingUpdate(float dt)
{
  if (mJumpPhase == 0)
    {
      mJumpPhaseElapsed -= dt;
      if (mJumpPhaseElapsed < 0.1f)
        {
          ++mJumpPhase;
          mBody->SetAwake(true);
          mBody->PutOnSleepOnCollision(false);

          float axisX =
              InputHandler::GetInstance()->GetAxis(GLFW_GAMEPAD_AXIS_LEFT_X);

          float axisY =
              InputHandler::GetInstance()->GetAxis(GLFW_GAMEPAD_AXIS_LEFT_Y);
          glm::vec3 dir =
              glm::normalize(glm::mat3(mOwner->Transform()->GetWorldMatrix()) *
                             glm::vec3(-axisX, 1.0f, -axisY));

          mBody->SetVelocity(dir * 350.0f);
          mBody->mGravityScale = 12.0f;
        }
    }
  if (mJumpPhase == 1 && mBody->GetVelocity().y < 0.0f)
    {

      mBody->PutOnSleepOnCollision(true);
      mJumpCastResult = mScene.GetPhysics().RayCast(
          mOwner->Transform()->Position() +
              glm::vec3(0.0f, mStandingMovementColliderSize.y, 0.0f),
          -mOwner->Transform()->up(), mStandingMovementColliderSize.y, 1);

      if (mJumpCastResult.success)
        {
          mPosition = mOwner->Transform()->Position();
          SetStance(EPlayerStance::STANDING_MOVEMENT);
          mLastPhase = 0;
          mBody->mGravityScale = 1.0f;
        }
    }
}

#ifdef EKDEBUG

void PlayerController::ImGuiProc()
{

}

void PlayerController::DebugDraw(
    Eklavya::Renderer::DebugRenderer &debugRenderer)
{

  if (mJumpPhase == 1 && mJumpCastResult.success)
    {

      debugRenderer.DrawLine(mOwner->Transform()->Position(),
                             mJumpCastResult.position,
                             glm::vec4(1.0f, 0.6, 0.6, 1.0f), 2.0f);
    }
}
#endif
