//
//  AninmationIKSolverComponent.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 04/01/2025.
//

#ifndef AninmationIKSolverComponent_hpp
#define AninmationIKSolverComponent_hpp

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "EkComponent.hpp"
#include "EkPhysics/PhysicsWorld.h"

namespace Eklavya
{

	class EkScene;

	namespace Renderer
	{
		class DebugRenderer;
	}

	class AnimationIKSolver : public EkComponent
	{
	  public:
		AnimationIKSolver(EkActor& owner, const EkScene& scene);
		~AnimationIKSolver(){}

		void Tick(float dt) override;

	  private:
		const std::string mLeftLegBoneName = "mixamorig_LeftToeBase";
		const std::string mRightLegBoneName = "mixamorig_RightToeBase";

		float mRayCastRange = 20.0f;

    glm::vec3 mLeftFootWorldPosition;
    glm::vec3 mRightFootWorldPosition;
		Eklavya::Physics::CastHitResult mLeftFootRayCastResult;
		Eklavya::Physics::CastHitResult mRightFootRayCastResult;

		const EkScene& mScene;

#ifdef EKDEBUG
		void DebugDraw(Renderer::DebugRenderer& renderer) override;
#endif
	};
} // namespace Eklavya
#endif /* AninmationIKSolverComponent_hpp */
