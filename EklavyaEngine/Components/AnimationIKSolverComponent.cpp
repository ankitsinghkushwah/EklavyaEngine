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
#include <AssetManager/Asset.h>
#include <glm/gtx/quaternion.hpp>

#include "AssetManager/AssetFactory.h"
#include "AssetManager/AssetManager.h"
#include "glm/gtc/matrix_access.hpp"
#include "glm/gtx/compatibility.hpp"
#include "glm/gtx/euler_angles.hpp"
#include "glm/gtx/vec_swizzle.hpp"
#include "SkeletalAnimationWithIK/PlayerController.hpp"

namespace Eklavya
{
	AnimationIKSolver::AnimationIKSolver(EkActor &owner, const EkScene &scene) :
		EkComponent(owner, CoreComponentIds::IK_SOLVER_COMPONENT), mScene(scene)
		, mPose(MAX_BONES_SUPPORTED, glm::mat4(1.0f))
		, mBoneWorldPoses(MAX_BONES_SUPPORTED, glm::mat4(1.0f))
		, mBasePose(MAX_BONES_SUPPORTED, glm::mat4(1.0f))

		, mLeftLegTargetPos(10.0f, 20.0f, 0.0f)

	{
		mLegsJointsData[0][0] = {"mixamorig_LeftUpLeg", glm::vec3(0.0f)};
		mLegsJointsData[0][1] = {"mixamorig_LeftLeg", glm::vec3(0.0f)};
		mLegsJointsData[0][2] = {"mixamorig_LeftToeBase", glm::vec3(0.0f)};

		mLegsJointsData[1][0] = {"mixamorig_RightUpLeg", glm::vec3(0.0f)};
		mLegsJointsData[1][1] = {"mixamorig_RightLeg", glm::vec3(0.0f)};
		mLegsJointsData[1][2] = {"mixamorig_RightToeBase", glm::vec3(0.0f)};


		const std::string name = "characters/swat/swat";
		mModel = AssetManager::GetInstance().GetAsset<Asset::GLModel>(name).get();
		mModelID = mModel->m_ModelID;
		mRoot = mModel->mRootNode->mTransform;
		mRootInverse = glm::inverse(mRoot);

		mAnimationComponent = GetOwner().GetComponent<AnimationComponent>(CoreComponentIds::ANIMATION_COMPONENT_ID);
	}

	void AnimationIKSolver::Tick(float dt)
	{
		float s = 20.0f;
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_H))
		{
			mLeftLegTargetPos.y += dt * s;
		}
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_J))
		{
			mLeftLegTargetPos.y -= dt * s;
		}
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_K))
		{
			mLeftLegTargetPos.z += dt * s;
		}
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_L))
		{
			mLeftLegTargetPos.z -= dt * s;
		}

		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_SPACE))
		{
			mLeftLegTargetPos = glm::vec3(10.0f, 20.0f, 0.0f);
		}

		Renderer::DebugRenderer::GetInstance().ClearAddedShapes();

		mBoneWorldPoses = std::vector<glm::mat4>(MAX_BONES_SUPPORTED, glm::mat4(1.0f));
		mPose = std::vector<glm::mat4>(MAX_BONES_SUPPORTED, glm::mat4(1.0f));

		const auto &modelDataMap = ModelsBoneData::s_BonesDataMap[mModelID];
		const auto &boneInfoMap = modelDataMap.m_BoneInfoMap;

		AdvanceBones(*mModel->mRootNode, glm::mat4(1.0f),
		             false);

		Solve(boneInfoMap);

		glm::vec3 pos = GetOwner().Transform().Position();
		mRoot = mModel->mRootNode->mTransform;
		mRootInverse = glm::inverse(mRoot);

		AdvanceBones(*mModel->mRootNode, glm::mat4(1.0f),
		             true);
	}

	void AnimationIKSolver::AdvanceBones(const Asset::ModelNode &node, glm::mat4 parentTransform, bool solve)
	{
		const auto &modelDataMap = ModelsBoneData::s_BonesDataMap[mModelID];
		const auto &boneInfoMap = modelDataMap.m_BoneInfoMap;

		std::string nodeName = node.mName;
		glm::mat4 nodeTransform = node.mTransform;
		glm::mat4 globalTransform = parentTransform * nodeTransform;

		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			glm::mat4 localTransform = glm::mat4(1.0f);
			const JointData *data = GetJointData(nodeName);
			if (data != nullptr)
			{
				localTransform = data->finalTransform;
				if (data->overrideTransform)
				{
					localTransform[3] = nodeTransform[3];
					nodeTransform = localTransform;
				}
				else
				{
					nodeTransform = nodeTransform * localTransform;
				}
			}
			else
				nodeTransform = nodeTransform * localTransform;

			globalTransform = parentTransform * nodeTransform;

			const int index = boneInfoMap.at(nodeName).id;
			glm::mat4 offset = boneInfoMap.at(nodeName).offset;

			mPose[index] = mRootInverse * globalTransform *
			               offset;
			mBoneWorldPoses[index] = GetOwner().Transform().GetWorldMatrix() * mRootInverse * globalTransform;
			glm::mat4 worldTransform = GetOwner().Transform().GetWorldMatrix() * mRootInverse * globalTransform;

			if (solve)
			{
				glm::vec4 color = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
				Renderer::DebugRenderer::GetInstance().AddSphere(worldTransform[3], .3,
				                                                 color);
			}
		}


		for (int i = 0; i < node.mChildren.size(); i++)
			AdvanceBones(*node.mChildren[i],
			             globalTransform, solve);
	}


	void AnimationIKSolver::Solve(const std::map<std::string, BoneInfo> &boneInfoMap)
	{
		//Raycast to determine target position for both foots

		mLeftFootWorldPosition = mBoneWorldPoses.at(boneInfoMap.at(mLeftLegBoneName).id)[3];
		mRightFootWorldPosition = mBoneWorldPoses.at(boneInfoMap.at(mRightLegBoneName).id)[3];
		const glm::vec3 rayDir(0.0f, -1.0f, 0.0f);

		mLeftFootRayCastResult = mScene.GetPhysics().RayCast(mLeftFootWorldPosition,
		                                                     rayDir, mRayCastRange, -1);
		mRightFootRayCastResult = mScene.GetPhysics().RayCast(mRightFootWorldPosition,
		                                                      rayDir, mRayCastRange, -1);

		for (int legIdx = 0; legIdx < 2; ++legIdx)
		{
			for (int jointIdx = 0; jointIdx < 3; ++jointIdx)
			{
				JointData &data = mLegsJointsData[legIdx][jointIdx];
				if (boneInfoMap.find(data.boneName) != boneInfoMap.end())
				{
					int idx = boneInfoMap.at(data.boneName).id;

					data.worldTransform = mBoneWorldPoses[idx];
					data.baseWorldTransform = mBasePose[idx];
					data.position = data.worldTransform[3];
					data.finalTransform = glm::mat4(1.0f);
					data.overrideTransform = false;
				}
				else
					assert(false);
			}
		}


		float offset = 0.0f;

		if (mLeftFootRayCastResult.success)
		{
			glm::vec3 targetPos = mLeftFootRayCastResult.position + mLeftFootRayCastResult.normal * offset;
			Solve2DForLeg(mLegsJointsData[0], mLeftLegTargetPos, true);
		}

		if (mRightFootRayCastResult.success)
		{
			glm::vec3 targetPos = mRightFootRayCastResult.position + mRightFootRayCastResult.normal * offset;
			Solve2DForLeg(mLegsJointsData[1], targetPos, false);
		}

		//AdjustFoot();
	}

	void AnimationIKSolver::AdjustFoot()
	{
		JointData &leftHip = mLegsJointsData[0][0];
		JointData &leftFoot = mLegsJointsData[0][2];
		JointData &rightFoot = mLegsJointsData[1][2];


		if (mLeftFootRayCastResult.success)
		{
			glm::vec3 groundNormal = mLeftFootRayCastResult.normal;

			glm::vec3 front = glm::normalize(glm::column(GetOwner().Transform().GetWorldMatrix(), 1));
			glm::quat rotation = glm::quatLookAtRH(front, glm::vec3(0.0f, 1.0f, 0.0f));

			leftFoot.overrideTransform = true;
			leftFoot.finalTransform = glm::toMat4(rotation);
		}
	}

	glm::vec3 GetEuler(const glm::mat4 &matrix)
	{
		// Extract quaternion from the matrix
		glm::quat rotation = glm::quat_cast(matrix);
		return glm::eulerAngles(rotation);
	}


	void AnimationIKSolver::Solve2DForLeg(std::array<JointData, 3> &legJoints, glm::vec3 targetPos, bool leftLeg)
	{
		JointData &Joint0 = legJoints[0];
		JointData &Joint1 = legJoints[1];
		JointData &Hand = legJoints[2];

		float offset = glm::pi<float>() / 2.0f;

		auto SetJointRotations = [&](float hipAngle, float kneeAngle)
		{
			glm::vec3 axisOfRotation = glm::vec3(1.0f, 0.0f, 0.0f);
			Joint0.boneAngle = hipAngle;
			Joint1.boneAngle = kneeAngle;
			Joint0.finalTransform = glm::rotate(glm::mat4(1.0f), hipAngle, axisOfRotation);
			Joint1.finalTransform = glm::rotate(glm::mat4(1.0f), kneeAngle, axisOfRotation);
		};

		float length0 = glm::distance(Joint0.position, Joint1.position);
		float length1 = glm::distance(Joint1.position, Hand.position);
		float length2 = glm::distance(Joint0.position, targetPos);

		glm::vec3 diff = targetPos - Joint0.position;
		float atan = glm::atan2(diff.y, diff.z);


		float hipAngle = 0.0f;
		float kneeAngle = 0.0f;

		if (length0 + length1 < length2)
		{
			hipAngle = atan;
			kneeAngle = 0.0f;

			SetJointRotations(offset + hipAngle, kneeAngle);

			return;
		}

		float cosAngle0 = ((length2 * length2) + (length0 * length0) - (length1 * length1)) / (2 * length2 * length0);
		float angle0 = glm::acos(cosAngle0);

		float cosAngle1 = ((length1 * length1) + (length0 * length0) - (length2 * length2)) / (2 * length1 * length0);
		float angle1 = glm::acos(cosAngle1);

		hipAngle = atan + angle0;
		kneeAngle = glm::pi<float>() + angle1;

		SetJointRotations(offset + hipAngle, kneeAngle);
	}

	void AnimationIKSolver::ImGuiProc() {}

	void AnimationIKSolver::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		bool disableDepthTest = true;
		if (mLeftFootRayCastResult.success)
		{
			debugRenderer.DrawLine(mLeftFootWorldPosition, mLeftFootRayCastResult.position,
			                       glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 0.2f, disableDepthTest);
			debugRenderer.DrawSphere(mLeftFootRayCastResult.position, .5f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
		}
		else
		{
			debugRenderer.DrawLine(mLeftFootWorldPosition,
			                       mLeftFootWorldPosition + glm::vec3(0.0f, -mRayCastRange, 0.0f),
			                       glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			                       .2f,
			                       disableDepthTest);
		}

		if (mRightFootRayCastResult.success)
		{
			debugRenderer.DrawLine(mRightFootWorldPosition, mRightFootRayCastResult.position,
			                       glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), .2f, disableDepthTest);
			debugRenderer.DrawSphere(mRightFootRayCastResult.position, .5f, glm::vec4(0.0f, 0.0f, 1.0f, 1.0f));
		}
		else
		{
			debugRenderer.DrawLine(mRightFootWorldPosition,
			                       mRightFootWorldPosition + glm::vec3(0.0f, -mRayCastRange, 0.0f),
			                       glm::vec4(0.0f, 0.0f, 0.0f, 1.0f),
			                       .2f,
			                       disableDepthTest);
		}

		mLeftFootRayCastResult.success = false;
		mRightFootRayCastResult.success = false;


		glm::vec3 pos = GetOwner().Transform().Position();


		debugRenderer.DrawSphere(mLeftLegTargetPos, 2.0f, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));
	}

	bool AnimationIKSolver::ContainsJoint(const std::string &name) const
	{
		const JointData *data = GetJointData(name);
		return data;
	}

	glm::mat4 AnimationIKSolver::GetJointTransform(const std::string &name) const
	{
		const JointData *data = GetJointData(name);
		if (data == nullptr)
			return glm::mat4(1.0f);
		return data->finalTransform;
	}

	const JointData *AnimationIKSolver::GetJointData(const std::string &name) const
	{
		for (int legIdx = 0; legIdx < 2; ++legIdx)
		{
			for (int jointIdx = 0; jointIdx < 3; ++jointIdx)
			{
				if (mLegsJointsData[legIdx][jointIdx].boneName == name)
					return &mLegsJointsData[legIdx][jointIdx];
			}
		}
		return nullptr;
	}
} // namespace Eklavya
