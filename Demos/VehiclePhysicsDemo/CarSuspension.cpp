//
//  CarSuspension.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 18/06/2023.
//

#include "CarSuspension.hpp"
#include <EkPhysics/EkBody.h>
#include <Components/TransformComponent.h>
#include "GLFWGame.h"
#include <Scene/EkActor.h>
#include <Scene/EkScene.h>
#include <EkPhysics/PhysicsWorld.h>
#include <Renderer/DebugRenderer.hpp>

#include "glm/gtx/vec_swizzle.hpp"

using namespace Eklavya;
using namespace Eklavya::Physics;

CarSuspension::CarSuspension(EkActor &owner, const EkScene *scene, int tireIdx)
  :
  EkComponent(owner, VehiclePhysicsComponentIds::SUSPENSION_COMPONENT_ID), mScene(scene)
{
  mID = tireIdx;


  mForcesColours[EWheelForces::SUSPSENSION] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
  mForcesColours[EWheelForces::DESIRED_DIRECTION] =
      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  mForcesColours[EWheelForces::VELOCITY] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
  mForcesColours[EWheelForces::UP_VELOCITY] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
  mForcesColours[EWheelForces::RIGHT_VELOCITY] =
      glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

  mChassisBody = owner.Parent()->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
}

void CarSuspension::Tick(float deltaTime)
{
  glm::vec3 v = mChassisBody->GetVelocityAtPoint(GetOwner().Transform().Position());
  if (mID < 2)
  {
    for (auto &track: mTracks)
    {
      track.elapsedTime += deltaTime;
    }

    auto end_from =
        std::remove_if(mTracks.begin(), mTracks.end(), [&](const TrackData &a)
        {
          return a.elapsedTime > mTracksLength;
        });

    // erasing the elements from new logical end
    mTracks.erase(end_from, mTracks.end());

    float velRight = glm::dot(GetOwner().Transform().right(), v);

    if (glm::abs(velRight) > 40.0f && mSkipSkipCounter++ == 5)
    {
      mSkipSkipCounter = 0;
      TrackData data;
      data.pos = hitResult.position;
      data.dir = mChassisBody->GetOwner().Transform().right();
      mTracks.push_back(data);
    }
  }

  if (mWheel)
  {
    float speed = glm::length(v);

    glm::vec3 start = GetOwner().Transform().Position();
    glm::vec3 dir = glm::normalize(hitResult.position - start);
    glm::vec3 wheelPos = hitResult.position - (dir * mWheelRadius);
    mWheel->Transform().SetPosition(wheelPos);

    glm::vec3 gv(v.x, 0.0f, v.z);
    float sign = glm::dot(GetOwner().Transform().forward(), gv) > 0.8 ? 1 : -1;

    mWheelAngle += sign * speed * 50.0f * deltaTime;

    glm::quat pRot = GetOwner().Transform().Rotation();
    glm::quat finalRot =
        pRot * glm::quat(glm::vec3(glm::radians(mWheelAngle), 0.0f, 0.0f));

    mWheel->Transform().SetRotation(finalRot);
  }
}

void ApplyImpulseAtPoint(EkBody &chassis, CarComponent &car, glm::vec3 J, glm::vec3 point)
{
  glm::vec3 linearComp = glm::vec3(0.0f);
  glm::vec3 angularComp = glm::vec3(0.0f);

  chassis.CalculateImpulseComponentsForPoint(J, point, linearComp,
                                             angularComp);
  car.mAccumulatedLinearVel += linearComp;
  car.mAccumulatedAngularVel += angularComp;
}


void CarSuspension::FixedTick(float dt)
{
  hitResult = mScene->GetPhysics().SphereCast(GetOwner().Transform().Position(),
                                              -GetOwner().Transform().up(),
                                              mWheelRadius, mSpringLength, 1);

  if (hitResult.success == true)
  {
    // suspension force
    glm::vec3 rayDirection = glm::normalize(GetOwner().Transform().up());

    glm::vec3 vel =
        mChassisBody->GetVelocityAtPoint(GetOwner().Transform().Position());
    float v = dot(vel, rayDirection);
    float offset = (mSpringLength - hitResult.t);
    float J = ((offset * mSpringStrength) - (v * mSpringDamping));

    mForces[EWheelForces::SUSPSENSION] = rayDirection * J;
    ApplyImpulseAtPoint(*mChassisBody, *mCarComponent, mForces[EWheelForces::SUSPSENSION],
                        GetOwner().Transform().Position());

    ApplyFrictionForces(dt);

    if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_UP))
    {
      ApplyImpulseAtPoint(*mChassisBody, *mCarComponent, -GetOwner().Transform().forward() * mTireSpeed,
                          GetOwner().Transform().Position());
    }
    else if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_DOWN))
    {
      ApplyImpulseAtPoint(*mChassisBody, *mCarComponent, GetOwner().Transform().forward() * mTireSpeed,
                          GetOwner().Transform().Position());
    }
  }

  UpdateSteering(dt);
}


void CarSuspension::ApplyFrictionForces(float dt)
{
  float lateralFriction = .1f;
  float longitudinalFriction = 0.1f;

  glm::vec3 vel =
      mChassisBody->GetVelocityAtPoint(GetOwner().Transform().Position());


  // adjusting the right force
  glm::vec3 right = GetOwner().Transform().right();

  float velRight = glm::dot(right, vel);
  float desVelRight = -velRight * lateralFriction;
  float accel = desVelRight;
  ApplyImpulseAtPoint(*mChassisBody, *mCarComponent, mTireMass * accel * right,
                      GetOwner().Transform().Position());

  glm::vec3 forward = GetOwner().Transform().forward();

  float velForward = glm::dot(forward, vel);
  float desVelForward = -velForward * longitudinalFriction;
  accel = desVelForward;
  ApplyImpulseAtPoint(*mChassisBody, *mCarComponent, mTireMass * accel * forward,
                      GetOwner().Transform().Position());
}


void CarSuspension::UpdateSteering(float dt)
{
  float steerAngle = 30.0f;

  if (mID > 1)
  {
    if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_RIGHT))
    {
      GetOwner().Transform().SetRotation(
        glm::vec3(0.0f, -steerAngle, 0.0f));
    }
    else if (InputHandler::GetInstance()->KeyHasPressed(
      GLFW_KEY_LEFT))
    {
      GetOwner().Transform().SetRotation(
        glm::vec3(0.0f, steerAngle, 0.0f));
    }
    else
    {
      GetOwner().Transform().SetRotation(glm::vec3(0.0f, 0.0f, 0.0f));
    }
  }
}


void CarSuspension::DebugDraw(Eklavya::Renderer::DebugRenderer &debugRenderer)
{
  for (auto &tracks: mTracks)
  {
    debugRenderer.DrawLine(tracks.pos,
                           tracks.pos - (tracks.dir * mTracksLength),
                           glm::vec4(0.2f, 0.2f, 0.2f, .3f), 3.0f);
  }
}

