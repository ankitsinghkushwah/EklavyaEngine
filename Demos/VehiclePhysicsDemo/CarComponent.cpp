//
//  CarComponent.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 18/06/2023.
//

#include "CarComponent.hpp"
#include "EkBody.h"
#include "TransformComponent.h"
#include "GLFWGame.h"
#include "EkActor.h"
#include "EkScene.h"

using namespace Eklavya;
using namespace Eklavya::Physics;

CarComponent::CarComponent()
    : EkComponent("CarComponent"),
      mEngineLoopSound("VehiclePhysicsDemo/car_engine_loop.mp3") {}

void CarComponent::Init() {
  mBody = mOwner->GetComponent<EkBody>("EkBody");

  mAudio.Load({mEngineLoopSound});
  mAudio.GetSound().setRelativeToListener(false);
  mAudio.GetSound().setVolume(100.0f);
  mAudio.GetSound().setMinDistance(400.0f);
  mAudio.GetSound().setAttenuation(5.0f);
  mAudio.Play(mEngineLoopSound);
}

float remap(float value, float oldMin, float oldMax, float newMin,
            float newMax) {
  // Normalize the value to the range [0, 1] within the original range
  float normalizedValue = (value - oldMin) / (oldMax - oldMin);

  // Remap the normalized value to the new range
  return glm::mix(newMin, newMax, normalizedValue);
}

void CarComponent::Tick(float dt) {
  if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_R)) {
    mBody->Clear();
    mBody->SetPos(glm::vec3(0.0f, 100.0f, -50.0f));
    mBody->SetRotation(glm::vec3(0.0f));
  }

  mAudio.Update(mOwner->Transform()->Position(), dt);

  float speed = glm::length(mBody->GetVelocity());
  float normalized = remap(speed, 0, 50.0f, 0.3, 1);
  mAudio.GetSound().setPitch(normalized);
  mAudio.GetSound().setVolume(normalized * 100);
}

#ifdef EKDEBUG

void CarComponent::DebugDraw(Eklavya::Renderer::DebugRenderer &debugRenderer) {}

#endif
