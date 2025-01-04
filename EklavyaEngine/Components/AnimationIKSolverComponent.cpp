//
//  AninmationIKSolverComponent.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 04/01/2025.
//

#include "AnimationIKSolverComponent.hpp"
#include "EkActor.h"
#include "Renderer/DebugRenderer.hpp"
#include "AnimationComponent.h"
#include "Scene/EkScene.h"

namespace Eklavya
{
	AnimationIKSolver::AnimationIKSolver(EkActor& owner, const EkScene& scene) : EkComponent(owner, CoreComponentIds::IK_SOLVER_COMPONENT), mScene(scene)
	{
	}

	void AnimationIKSolver::Tick(float dt)
	{
		AnimationComponent* animationComponent = GetOwner().GetComponent<AnimationComponent>(CoreComponentIds::ANIMATION_COMPONENT_ID);
		if (SHARED_ANIMATION currAnimation = animationComponent->GetCurrentAnimation())
		{
			mLeftFootWorldPosition = animationComponent->GetBoneTransform(mLeftLegBoneName)[3];
			mRightFootWorldPosition = animationComponent->GetBoneTransform(mRightLegBoneName)[3];

			const glm::vec3 rayDir(0.0f, -1.0f, 0.0f);
			mLeftFootRayCastResult = mScene.GetPhysics().RayCast(mLeftFootWorldPosition, rayDir, mRayCastRange, -1);
			mRightFootRayCastResult = mScene.GetPhysics().RayCast(mRightFootWorldPosition, rayDir, mRayCastRange, -1);
		}
	}

#ifdef EKDEBUG
	void AnimationIKSolver::DebugDraw(Renderer::DebugRenderer& debugRenderer)
	{
		if (mLeftFootRayCastResult.success)
		{
			debugRenderer.DrawLine(mLeftFootWorldPosition, mLeftFootRayCastResult.position, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 2.0f);
			debugRenderer.DrawSphere(mLeftFootRayCastResult.position, 5.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		}
		else
		{
			debugRenderer.DrawLine(mLeftFootWorldPosition, mLeftFootWorldPosition + glm::vec3(0.0f, -mRayCastRange, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
		}

		if (mRightFootRayCastResult.success)
		{
			debugRenderer.DrawLine(mRightFootWorldPosition, mRightFootRayCastResult.position, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 2.0f);
			debugRenderer.DrawSphere(mRightFootRayCastResult.position, 5.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		}
		else
		{
			debugRenderer.DrawLine(mRightFootWorldPosition, mRightFootWorldPosition + glm::vec3(0.0f, -mRayCastRange, 0.0f), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), 2.0f);
		}
  
  mLeftFootRayCastResult.success = false;
  mRightFootRayCastResult.success = false;
	}
#endif
} // namespace Eklavya
