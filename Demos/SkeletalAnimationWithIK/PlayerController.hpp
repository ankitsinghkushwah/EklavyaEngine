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
	class EkActor;
}; // namespace Eklavya

namespace Eklavya::Renderer
{
	class DebugRenderer;
}

enum EPlayerStance
{
	STANDING_MOVEMENT,
	CROUCH_MOVEMENT,
	PS_MAX
};

enum EMovementDirection
{
	MD_IDLE,
	MD_FORWARD,
  MD_BACKWARD,
	MD_TURN_LEFT,
	MD_TURN_RIGHT,
	MD_MAX
};

class PlayerController
    : public UserInputListener
    , public Eklavya::EkComponent
{
  public:
	PlayerController(Eklavya::EkActor& owner, Eklavya::EkScene&);

	void Init();

	void SetStance(EPlayerStance stance);
	void SetDirection(EMovementDirection direction);
	void Tick(float dt) override;

	void StanceUpdate(float dt);
	void TurnUpdate(float dt);

	//TURN UPDATE
	void TurnStart();

#ifdef EKDEBUG
	void ImGuiProc();
	void DebugDraw(Eklavya::Renderer::DebugRenderer&) override;
#endif

	void OnKeyAction(int key, int action) override;

  private:

	void        LoadAnimations();
	std::string GetDirectionString(EPlayerStance stance,EMovementDirection direction);

	
	EMovementDirection mLastDirection;

	Eklavya::EkScene&  mScene;
	EPlayerStance      mStance;
	EMovementDirection mDirection;

	// Actor components

	Eklavya::AnimationComponent* mAnimator = nullptr;
	// Standing Movement Speed Animations
	glm::vec3                                                                                   mStandingMovementColliderSize;
	std::array<std::array<SHARED_ANIMATION, EMovementDirection::MD_MAX>, EPlayerStance::PS_MAX> mStandingMovementAnims; // idle, walk, turn left, turn right

  // Crouch
  bool mCrouchToggle = false;
	float mCrouchTime = 0.0f;
	float mCrouchElapsed = 0.0f;

	// params
	float mSpeed = 250.0f;

	//rotation
	bool  mShouldTurnRight = false;
	float mRotationSpeed = 100.0f;
	float mRotationAngle = 0.0f;
	float mRotationTime = 1.0f;
	float mRotationTimeElapsed = 0.0f;
	bool  mTurning = false;

	glm::vec3 mPosition;
	float     mGravity = -250.0f;
	glm::vec3 mCurrGravityDir;

	Eklavya::Physics::CastHitResult mCastResult;
	float                           mCastRange = 20.0f;
 
  //Left
};

#endif /* State_hpp */
