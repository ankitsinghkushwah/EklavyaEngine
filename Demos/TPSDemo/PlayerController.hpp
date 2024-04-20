//
//  State.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/04/2024.
//

#ifndef PlayerController_hpp
#define PlayerController_hpp

#include "EkActor.h"
#include "Components/AnimationComponent.h"
#include "AssetManager/Asset.h"

#include "EkPhysics/EkBody.h"
#include "EkPhysics/Collider.h"
#include "EkPhysics/PhysicsWorld.h"

#include <unordered_map>

namespace Eklavya
{
  class EkScene;
};

namespace Eklavya::Renderer
{
  class DebugRenderer;
}

enum EPlayerStance
{
  STANDING_MOVEMENT,
  STAND_SHOOTING,
  CROUCH_MOVEMENT,
  CROUCH_SHOOTING,
  CRAWL,
  DEAD,
  JUMPING
};

enum EMovementDirection
{
  MD_FORWARD,
  MD_BACKWARD,
  MD_LEFT,
  MD_RIGHT,
  MD_MAX
};

enum EShootingPhase
{
  AIM_IN,
  AIMING,
  SHOOTING,
  AIM_OUT
};

class PlayerController : public UserInputListener, public Eklavya::EkComponent
{
public:
  PlayerController(Eklavya::EkScene &);

  void Init() override;

  void SetStance(EPlayerStance stance);
  void SetDirection(EMovementDirection direction);
  void Tick(float dt) override;

  // STANCES FUNCTIONS

  void UpdateRotation(float dt);
  // STANDING MOVEMENT
  void StandingMovementStart();
  void StandingMovementUpdate(float dt);
  
  // STANDING MOVEMENT
  void JumpingStart();
  void JumpingUpdate(float dt);

  // STANDING SHOOTING
  void StandShootingStart();
  void StandShootUpdate(float dt);
  
  //JUMPING

  // CROUCH SHOOTING
  void CrouchShootingStart();
  void CrouchShootingUpdate(float dt);

#ifdef EKDEBUG
  void ImGuiProc();
  void DebugDraw(Eklavya::Renderer::DebugRenderer &) override;
#endif
  
  void OnKeyAction(int key, int action);

private:
  bool CanSwitchToStance(EPlayerStance stance);
  void               LoadAnimations();
  std::string        GetDirectionString(EMovementDirection direction);

  EMovementDirection GetDirection(float axisX, float axisY);
  int                GetPhase(float speed);

  int                mLastPhase = 0;
  EMovementDirection mLastDirection;
  glm::vec3          mLastPosition;

  Eklavya::EkScene  &mScene;
  EPlayerStance      mStance;
  EMovementDirection mDirection;

  // Actor components
  Eklavya::Physics::SHARED_EKBODY                mBody = nullptr;
  Eklavya::SHARED_ANIMATOR                       mAnimator = nullptr;
  std::shared_ptr<Eklavya::Physics::BoxCollider> mBoxCollider = nullptr;

  // Standing Movement Speed Animations
  glm::vec3 mStandingMovementColliderSize;
  std::array<std::array<SHARED_ANIMATION, EMovementDirection::MD_MAX>, 3>
      mStandingMovementAnims; // idle, walk and run
      
      
  //Jumping , takoeff,flight and land
  std::array<SHARED_ANIMATION,3> mJumpPhaseAnims;
  float mJumpTakeOffTime = 0.0f;
  float mJumpPhaseElapsed = 0.0f;
  float mJumpLandingTime = 0.0f;
  int mJumpPhase = 0;
  Eklavya::Physics::CastHitResult mJumpCastResult;

  // Standing shooting
  
  float            mStandingFireTime = 0.0f;
  float            mStandingFireElapsed = 0.0f;
  SHARED_ANIMATION mStandShootingAnimation = nullptr;
  

  // Crouch shooting
  float            mCrouchFireTime = 0.0f;
  float            mCrouchFireElapsed = 0.0f;
  SHARED_ANIMATION mCrouchShootingAnim = nullptr;

  // params
  float     mSpeed = 500.0f;
  float     mRotationSpeed = 100.0f;
  glm::vec3 mPosition;
  float     mAngleOffset = 0.0f;
};

DECLARE_COMPONENT_ESSENTIALS(PlayerController, PLAYER_CONTROLLER);

#endif /* State_hpp */
