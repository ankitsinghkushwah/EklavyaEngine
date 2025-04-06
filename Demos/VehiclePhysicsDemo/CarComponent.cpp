//
//  CarComponent.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 18/06/2023.
//

#include "CarComponent.hpp"
#include <EkPhysics/EkBody.h>
#include <Components/TransformComponent.h>
#include "GLFWGame.h"
#include <Scene/EkActor.h>
#include <Scene/EkScene.h>

using namespace Eklavya;
using namespace Eklavya::Physics;

CarComponent::CarComponent(Eklavya::EkActor &owner) :
	EkComponent(owner, VehiclePhysicsComponentIds::CAR_COMPONENT_ID),
	mEngineLoopSound("VehiclePhysicsDemo/car_engine_loop.mp3")
	, mChassisBody(owner.GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID))
	, mAccumulatedLinearVel(0.0f), mAccumulatedAngularVel(0.0f)
{
	mAudio.Load({mEngineLoopSound});
	mAudio.GetSound().setRelativeToListener(false);
	mAudio.GetSound().setVolume(100.0f);
	mAudio.GetSound().setMinDistance(400.0f);
	mAudio.GetSound().setAttenuation(5.0f);
	mAudio.Play(mEngineLoopSound);
}

float remap(float value, float oldMin, float oldMax, float newMin, float newMax)
{
	float normalizedValue = (value - oldMin) / (oldMax - oldMin);
	return glm::mix(newMin, newMax, normalizedValue);
}

void CarComponent::Tick(float dt)
{
	if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_R))
	{
		mChassisBody->Clear();
		mChassisBody->SetPosition(glm::vec3(0.0f, 100.0f, 0.0f));
		mChassisBody->SetRotation(glm::vec3(0.0f));
	}


	mAudio.Update(GetOwner().Transform().Position(), dt);

	float speed = glm::length(mChassisBody->GetVelocity());
	float normalized = remap(speed, 0, 50.0f, 0.3, 1);
	mAudio.GetSound().setPitch(normalized);
	mAudio.GetSound().setVolume(normalized * 100);
}

void CarComponent::FixedTick(float dt)
{
	mChassisBody->ApplyAccumulatedImpulse(mAccumulatedLinearVel, mAccumulatedAngularVel);
	mAccumulatedLinearVel = glm::vec3(0.0f);
	mAccumulatedAngularVel = glm::vec3(0.0f);
}

#ifdef EKDEBUG

void CarComponent::DebugDraw(Eklavya::Renderer::DebugRenderer &debugRenderer) {}

#endif
