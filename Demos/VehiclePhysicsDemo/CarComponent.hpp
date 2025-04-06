//
//  CarComponent.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 18/06/2023.
//

#ifndef INC_CAR
#define INC_CAR

#include <Components/EkComponent.hpp>
#include "UserInputListener.h"
#include <glm/glm.hpp>
#include <EkPhysics/PhysicsWorld.h>
#include <Scene/EkActor.h>
#include "EkAudio.hpp"

namespace Eklavya::Physics
{
	class EkBody;
};

namespace Eklavya
{
	class EkActor;
	class EkScene;
	class TransformComponent;
} // namespace Eklavya

struct CarComponent : public Eklavya::EkComponent
{
	CarComponent(Eklavya::EkActor &owner);

	void Tick(float dt) override;

	void FixedTick(float dt) override;

#ifdef EKDEBUG
	void DebugDraw(Eklavya::Renderer::DebugRenderer &debugRenderer) override;
#endif
	Eklavya::Audio mAudio;
	std::string mEngineLoopSound;

	glm::vec3 mAccumulatedLinearVel;
	glm::vec3 mAccumulatedAngularVel;

	Eklavya::Physics::EkBody *mChassisBody = nullptr;
};

#endif /* CarComponent_hpp */
