//
//  AninmationIKSolverComponent.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 04/01/2025.
//

#include "AnimationIKSolverComponent.hpp"
#include <Scene/EkActor.h>
#include <Renderer/DebugRenderer.hpp>
#include "AnimationComponent.h"
#include <Scene/EkScene.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Eklavya
{
	AnimationIKSolver::AnimationIKSolver(EkActor& owner, const EkScene& scene) : EkComponent(owner, CoreComponentIds::IK_SOLVER_COMPONENT), mScene(scene)
	{
		mLegsJointsData[0][0] = {"mixamorig_LeftUpLeg", glm::vec3(0.0f)};
		mLegsJointsData[0][1] = {"mixamorig_LeftLeg", glm::vec3(0.0f)};
		mLegsJointsData[0][2] = {"mixamorig_LeftToeBase", glm::vec3(0.0f)};

		mLegsJointsData[1][0] = {"mixamorig_RightUpLeg", glm::vec3(0.0f)};
		mLegsJointsData[1][1] = {"mixamorig_RightLeg", glm::vec3(0.0f)};
		mLegsJointsData[1][2] = {"mixamorig_RightToeBase", glm::vec3(0.0f)};
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

	bool AnimationIKSolver::ContainsJoint(const std::string& name) const
	{
		const JointData* data = GetJointData(name);
		return data;
	}

	glm::mat4 AnimationIKSolver::GetJointTransform(const std::string& name) const
	{
		const JointData* data = GetJointData(name);
		if (data == nullptr)
			return glm::mat4(1.0f);
		return data->finalTransform;
	}
	const JointData* AnimationIKSolver::GetJointData(const std::string& name) const
	{
		for (int legIdx = 0; legIdx < 2; ++legIdx)
		{
			for (int jointIdx = 0; jointIdx < 2; ++jointIdx)
			{
				if (mLegsJointsData[legIdx][jointIdx].boneName == name)
					return &mLegsJointsData[legIdx][jointIdx];
			}
		}
		return nullptr;
	}
	void AnimationIKSolver::Solve(const std::map<std::string, BoneInfo>& boneInfoMap,
	                              const std::vector<glm::mat4>&          boneWorldTransforms,
	                              const std::vector<glm::mat4>&          boneLocalTransforms,
	                              const std::vector<glm::mat4>&          parentTransforms)
	{
		//Raycast to determine target position for both foots

		mLeftFootWorldPosition = boneWorldTransforms.at(boneInfoMap.at(mLeftLegBoneName).id)[3];
		mRightFootWorldPosition = boneWorldTransforms.at(boneInfoMap.at(mRightLegBoneName).id)[3];
		const glm::vec3 rayDir(0.0f, -1.0f, 0.0f);
		mLeftFootRayCastResult = mScene.GetPhysics().RayCast(mLeftFootWorldPosition, rayDir, mRayCastRange, -1);
		mRightFootRayCastResult = mScene.GetPhysics().RayCast(mRightFootWorldPosition, rayDir, mRayCastRange, -1);

		for (int legIdx = 0; legIdx < 2; ++legIdx)
		{
			for (int jointIdx = 0; jointIdx < 3; ++jointIdx)
			{
				JointData& data = mLegsJointsData[legIdx][jointIdx];
				if (boneInfoMap.find(data.boneName) != boneInfoMap.end())
				{
					int idx = boneInfoMap.at(data.boneName).id;
					data.sourceWorldTransform = boneWorldTransforms[idx];
					data.sourceLocalTransform = boneLocalTransforms[idx];
					data.parentTransform = parentTransforms[idx];
					data.position = data.sourceWorldTransform[3];
				}
				else
					assert(false);
			}
		}

		if (mLeftFootRayCastResult.success)
		{
			SolveForLeg(mLegsJointsData[0], mLeftFootRayCastResult.position, true);
		}

		if (mRightFootRayCastResult.success)
		{
			SolveForLeg(mLegsJointsData[1], mRightFootRayCastResult.position, false);
		}
	}

	glm::vec3 GetEuler(const glm::mat4& matrix)
	{
		// Extract quaternion from the matrix
		glm::quat rotation = glm::quat_cast(matrix);

		return glm::eulerAngles(rotation);
	}

	void AnimationIKSolver::SolveForLeg(std::array<JointData, 3>& legJoints, glm::vec3 targetPos, bool leftLeg)
	{
	}

#ifdef EKDEBUG
	void AnimationIKSolver::DebugDraw(Renderer::DebugRenderer& debugRenderer)
	{
		bool disableDepthTest = true;
		if (mLeftFootRayCastResult.success)
		{
			debugRenderer.DrawLine(mLeftFootWorldPosition, mLeftFootRayCastResult.position, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 2.0f, disableDepthTest);
			debugRenderer.DrawSphere(mLeftFootRayCastResult.position, 5.0f, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
		else
		{
			debugRenderer.DrawLine(mLeftFootWorldPosition,
			                       mLeftFootWorldPosition + glm::vec3(0.0f, -mRayCastRange, 0.0f),
			                       glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			                       2.0f,
			                       disableDepthTest);
		}

		if (mRightFootRayCastResult.success)
		{
			debugRenderer.DrawLine(mRightFootWorldPosition, mRightFootRayCastResult.position, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f), 2.0f, disableDepthTest);
			debugRenderer.DrawSphere(mRightFootRayCastResult.position, 5.0f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		}
		else
		{
			debugRenderer.DrawLine(mRightFootWorldPosition,
			                       mRightFootWorldPosition + glm::vec3(0.0f, -mRayCastRange, 0.0f),
			                       glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
			                       2.0f,
			                       disableDepthTest);
		}

		mLeftFootRayCastResult.success = false;
		mRightFootRayCastResult.success = false;
	}
#endif
} // namespace Eklavya
