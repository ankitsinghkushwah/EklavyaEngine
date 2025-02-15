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

using namespace Eklavya;
using namespace Eklavya::Physics;

CarSuspension::CarSuspension(EkActor& owner,const EkScene *scene, int tireIdx)
    : EkComponent(owner,VehiclePhysicsComponentIds::SUSPENSION_COMPONENT_ID), mScene(scene)
{
  mID = tireIdx;

 
  mSpringLength = 5.0f;
  mSpringStrength = 50.0f;
  mSpringDamping = 5.0f;

  mWheelAngle = 0.0f;
  mWheelRadius = 2.2f;
  mTireSpeed = 100.0f;

  mForcesColours[EWheelForces::SUSPSENSION] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
  mForcesColours[EWheelForces::DESIRED_DIRECTION] =
      glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
  mForcesColours[EWheelForces::VELOCITY] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
  mForcesColours[EWheelForces::UP_VELOCITY] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
  mForcesColours[EWheelForces::RIGHT_VELOCITY] =
      glm::vec4(1.0f, 1.0f, 0.0f, 1.0f);

    mChassisBody = owner.Parent()->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
}


bool sfxPlayed = false;

void CarSuspension::Tick(float deltaTime)
{

  for (auto &track : mTracks)
    {
      track.elapsedTime += deltaTime;
    }

  auto end_from =
      std::remove_if(mTracks.begin(), mTracks.end(), [&](const TrackData &a) {
        return a.elapsedTime > mTracksLength;
      });

  // erasing the elements from new logical end
  mTracks.erase(end_from, mTracks.end());

  // Set wheel position

  if (mWheel)
    {
      glm::vec3 start = GetOwner().Transform().Position();
      glm::vec3 dir = glm::normalize(hitResult.position - start);
      glm::vec3 wheelPos = hitResult.position - (dir * mWheelRadius);
      mWheel->Transform().SetPosition(wheelPos);
      // hitResult.position - (dir * radius);

      glm::vec3 v = mChassisBody->GetVelocity();
      glm::vec3 gv(v.x, 0.0f, v.z);
      float sign = glm::dot(GetOwner().Transform().forward(), gv) > 0.8 ? 1 : -1;

      float speed = glm::length(gv);
      mWheelAngle += sign * speed * 50.0f * deltaTime;

      glm::quat pRot = GetOwner().Transform().Rotation();
      glm::quat finalRot =
          pRot * glm::quat(glm::vec3(glm::radians(mWheelAngle), 0.0f, 0.0f));

      mWheel->Transform().SetRotation(finalRot);
    }
}

void CarSuspension::FixedTick(float dt)
{

  hitResult = mScene->GetPhysics().RayCast(GetOwner().Transform().Position(),
                                           -GetOwner().Transform().up(),
                                           mSpringLength, 1);

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
      mChassisBody->ApplyImpulseAtPoint(mForces[EWheelForces::SUSPSENSION],
                                        GetOwner().Transform().Position());

      float steerAngle = 30.0f;

      // front tires forces
      if (mID > 1)
        {
          if (InputHandler::GetInstance()->IsControllerConnected() == false)
            {
              // acceleraton/brake
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
          else
            {
              float axis = InputHandler::GetInstance()->GetAxis(
                  GLFW_GAMEPAD_AXIS_LEFT_X);
              GetOwner().Transform().SetRotation(
                  glm::vec3(0.0f, -axis * steerAngle, 0.0f));
            }
        }

      if (InputHandler::GetInstance()->IsControllerConnected() == false)
        {
          if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_UP))
            {
              mChassisBody->ApplyImpulseAtPoint(
                  -GetOwner().Transform().forward() * mTireSpeed,
                  GetOwner().Transform().Position());
            }

          if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_B))
            {
              mChassisBody->ApplyImpulseAtPoint(
                  -GetOwner().Transform().forward() * 100.0f,
                  GetOwner().Transform().Position());
            }

          if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_DOWN))
            {
              mChassisBody->ApplyImpulseAtPoint(
                  GetOwner().Transform().forward() * mTireSpeed,
                  GetOwner().Transform().Position());
            }
        }
      else
        {
          float axis = (InputHandler::GetInstance()->GetAxis(
                            GLFW_GAMEPAD_AXIS_LEFT_TRIGGER) +
                        1) *
                       0.5;
          mChassisBody->ApplyImpulseAtPoint(-GetOwner().Transform().forward() *
                                                axis * mTireSpeed,
                                            GetOwner().Transform().Position());

          float axis2 = (InputHandler::GetInstance()->GetAxis(
                             GLFW_GAMEPAD_AXIS_RIGHT_TRIGGER) +
                         1) *
                        0.5;
          mChassisBody->ApplyImpulseAtPoint(GetOwner().Transform().forward() *
                                                axis2 * mTireSpeed,
                                            GetOwner().Transform().Position());

          if (InputHandler::GetInstance()->KeyHasPressed(
                  GLFW_GAMEPAD_BUTTON_CROSS))
            {
              mChassisBody->ApplyImpulseAtPoint(
                  GetOwner().Transform().forward() * mTireSpeed,
                  GetOwner().Transform().Position());
            }
        }

      // adjusting the right force
      glm::vec3 right = GetOwner().Transform().right();

      float     velRight = glm::dot(right, vel);
      float     desVelRight = -velRight * 1.0f;
      float     accel = desVelRight / dt;
      mChassisBody->ApplyImpulseAtPoint(right * accel * dt,
                                        GetOwner().Transform().Position());

      glm::vec3 forward = GetOwner().Transform().forward();

      float     velForward = glm::dot(forward, vel);
      float     desVelForward = -velForward * 1.0f;
      accel = desVelForward / dt;
      mChassisBody->ApplyImpulseAtPoint(forward * accel * dt,
                                        GetOwner().Transform().Position());
    }
}

#ifdef EKDEBUG



void CarSuspension::DebugDraw(Eklavya::Renderer::DebugRenderer &debugRenderer)
{

glm::vec4 hitColor(1.0f, 0.0f, 0.0f, 0.5f);
glm::vec4 rayCol(0.0f,0.0f,0.0f,1.0f);

  glm::vec3 start = GetOwner().Transform().Position();

  debugRenderer.DrawLine(start, hitResult.position,rayCol, 0.2f);
  float     radius = .3f;
  glm::vec3 dir = glm::normalize(hitResult.position - start);
  glm::vec3 wheelPos = hitResult.position - (dir * radius);
  debugRenderer.DrawSphere(wheelPos, glm::vec3(radius),
                           hitColor);

//  for (int i = 0; i < MAX_WHEEL_FORCES; ++i)
//    {
//      debugRenderer.DrawLine(start, start + mForces[i], mForcesColours[i], .1f);
//    }

  for (auto &tracks : mTracks)
    {
      debugRenderer.DrawLine(tracks.pos,
                             tracks.pos - (tracks.dir * mTracksLength),
                             glm::vec4(0.6f, 0.4f, 0.2f, 1.0f), .1f);
    }
}

#endif
