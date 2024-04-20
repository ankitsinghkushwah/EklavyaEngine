//
//  CarComponent.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 18/06/2023.
//

#ifndef INC_SUSPENSION
#define INC_SUSPENSION

#include "EkComponent.hpp"
#include "UserInputListener.h"
#include <glm/glm.hpp>
#include "PhysicsWorld.h"
#include "EkActor.h"
#include "EkAudio.hpp"

namespace Eklavya::Physics
{
  class EkBody;
};

enum EWheelForces
{
  SUSPSENSION,
  DESIRED_DIRECTION,
  VELOCITY,
  UP_VELOCITY,
  RIGHT_VELOCITY,
  MAX_WHEEL_FORCES
};

namespace Eklavya
{
  class EkScene;
  class TransformComponent;
} // namespace Eklavya

struct TrackData
{
  float     elapsedTime;
  glm::vec3 dir;
  glm::vec3 pos;
};

struct CarSuspension : public Eklavya::EkComponent
{
  CarSuspension(const Eklavya::EkScene *scene, int tireIdx);

  void Init() override;
  void FixedTick(float fixedDeltaTime) override;
  void Tick(float deltaTime) override;

#ifdef EKDEBUG
  void DebugDraw(Eklavya::Renderer::DebugRenderer &debugRenderer) override;
#endif

  Eklavya::Physics::SHARED_EKBODY         mChassisBody;
  Eklavya::Physics::CastHitResult         hitResult;
  Eklavya::SHARED_ACTOR                   mWheel;
  const Eklavya::EkScene                 *mScene;

  std::array<glm::vec3, MAX_WHEEL_FORCES> mForces;
  std::array<glm::vec4, MAX_WHEEL_FORCES> mForcesColours;

  std::vector<TrackData>                  mTracks;
  float                                   mTracksDelay = 10.0f;
  float                                   mTracksLength = 4.0f;

  float                                   mSpringLength = 0.0f;
  float                                   mSpringStrength = 0.0f;
  float                                   mSpringDamping = 0.0f;
  float                                   mTireSpeed = 0.0f;

  float                                   mWheelAngle = 0.0f;
  float                                   mWheelRadius = 0.0f;

  int                                  mID = -1;
};

DECLARE_COMPONENT_ESSENTIALS(CarSuspension, SUSPENSION);

#endif
