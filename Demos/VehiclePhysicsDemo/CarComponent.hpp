//
//  CarComponent.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 18/06/2023.
//

#ifndef INC_CAR
#define INC_CAR

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

namespace Eklavya
{
	class EkActor;
	class EkScene;
	class TransformComponent;
} // namespace Eklavya

struct CarComponent : public Eklavya::EkComponent
{
	CarComponent(Eklavya::EkActor& owner);

	void Tick(float dt) override;

#ifdef EKDEBUG
	void DebugDraw(Eklavya::Renderer::DebugRenderer& debugRenderer) override;
#endif
	Eklavya::Audio mAudio;
	std::string    mEngineLoopSound;
};

#endif /* CarComponent_hpp */
