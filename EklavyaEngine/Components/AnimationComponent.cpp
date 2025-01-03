#include "AnimationComponent.h"
#include <AssetManager/AssetManager.h>
#include <AssetManager/GLModel.hpp>
#include <AssetManager/Animation.h>
#include <AssetManager/Bone.h>
#include <GLFW/glfw3.h>
#include "AnimationData.h"
#include "Bone.h"
#include "TransformComponent.h"
#include "EkActor.h"
#include "Renderer/DebugRenderer.hpp"
#include <functional>
#include "ComponentIds.h"

using namespace Eklavya::Asset;

namespace Eklavya
{
	AnimationComponent::AnimationComponent(EkActor& owner, ModelID modelId)
	    : EkComponent(owner, CoreComponentIds::ANIMATION_COMPONENT_ID)
	    , mDeltaTime(0.0f)
	    , mModelID(modelId)
	    , mPose(MAX_BONES_SUPPORTED, glm::mat4(1.0f))
	    , mLocalPose(MAX_BONES_SUPPORTED, glm::mat4(1.0f))
	{
		mPose.reserve(MAX_BONES_SUPPORTED);
		mCurrentAnimation = nullptr;
		mNextAnimation = nullptr;
		mIsSwitchingAnimation = false;
		mElapsedAnimTransition = 0.0f;
		mMaxAnimTransitionDuration = 1.0f;
		mTransitionSpeed = 10.0f;
		mCurrentTime = 0.0f;
	}

	AnimationComponent::~AnimationComponent()
	{
	}

	glm::mat4 AnimationComponent::InterpolateToNextAnimation(Bone* jointFrom, Bone* jointTo)
	{
		glm::mat4 result(1.0f);
		float     scaleFactor = mElapsedAnimTransition / (float)mMaxAnimTransitionDuration; // 0 - 1
		scaleFactor = glm::clamp(scaleFactor, 0.0f, 1.0f);
		result = LerpPos(jointFrom, jointTo, scaleFactor) * SlerpRot(jointFrom, jointTo, scaleFactor) * LerpScale(jointFrom, jointTo, scaleFactor);
		return result;
	}

	glm::mat4 AnimationComponent::LerpPos(Bone* jointFrom, Bone* jointTo, float scaleFactor)
	{
		int       p0Index = jointFrom->GetPositionIndex(mCurrentTime);
		int       p1Index = jointTo->GetPositionIndex(0);
		glm::vec3 finalPos = glm::mix(jointFrom->GetPosAtIndex(p0Index), jointTo->GetPosAtIndex(p1Index), scaleFactor);
		return glm::translate(glm::mat4(1.0), finalPos);
	}

	glm::mat4 AnimationComponent::SlerpRot(Bone* jointFrom, Bone* jointTo, float scaleFactor)
	{
		int       p0Index = jointFrom->GetRotationIndex(mCurrentTime);
		int       p1Index = jointTo->GetRotationIndex(0);
		glm::quat finalRotation = glm::slerp(jointFrom->GetRotAtIndex(p0Index), jointTo->GetRotAtIndex(p1Index), scaleFactor);
		finalRotation = glm::normalize(finalRotation);
		return glm::toMat4(finalRotation);
	}

	glm::mat4 AnimationComponent::LerpScale(Bone* jointFrom, Bone* jointTo, float scaleFactor)
	{
		int       p0Index = jointFrom->GetScaleIndex(mCurrentTime);
		int       p1Index = jointTo->GetScaleIndex(0);
		glm::vec3 finalScale = glm::mix(jointFrom->GetScaleAtIndex(p0Index), jointTo->GetScaleAtIndex(p1Index), scaleFactor);
		return glm::scale(glm::mat4(1.0), finalScale);
	}

	void AnimationComponent::PlayAnimation(std::shared_ptr<Animation> pAnimation, bool smoothTransition)
	{
		mElapsedAnimTransition = 0.0f;
		mCurrentTime = 0.0f;
		if (smoothTransition == false)
		{
			mCurrentAnimation = pAnimation;
			return;
		}
		if (!mCurrentAnimation)
		{
			mIsSwitchingAnimation = false;
			mCurrentAnimation = pAnimation;
		}
		else
		{
			mNextAnimation = pAnimation;
			mIsSwitchingAnimation = true;
		}
	}

	void AnimationComponent::Tick(float dt)
	{
		mDeltaTime = dt;

		if (!mIsSwitchingAnimation && mCurrentAnimation)
		{
			mCurrentTime += mCurrentAnimation->GetTicksPerSecond() * dt;
			mCurrentTime = fmod(mCurrentTime, mCurrentAnimation->GetDuration());
			AdvanceBones(&mCurrentAnimation->GetRootNode(), glm::mat4(1.0f));
		}
		else if (mIsSwitchingAnimation && mCurrentAnimation && mNextAnimation)
		{
			mElapsedAnimTransition += mTransitionSpeed * dt;
			AdvanceBones(&mCurrentAnimation->GetRootNode(), glm::mat4(1.0f));
			if (mElapsedAnimTransition > mMaxAnimTransitionDuration)
			{
				mIsSwitchingAnimation = false;
				mCurrentAnimation = mNextAnimation;
				mNextAnimation = nullptr;
			}
		}
	}

	void AnimationComponent::AdvanceBones(const AssimpNodeData* node, glm::mat4 parentTransform)
	{
		std::string nodeName = node->name;
		glm::mat4   nodeTransform = glm::mat4(1.0f);
		Bone*       joint = mCurrentAnimation->FindJoint(nodeName);
		if (joint)
		{
			if (mIsSwitchingAnimation)
			{
				Bone* jointTo = mNextAnimation->FindJoint(nodeName);
				if (jointTo)
					nodeTransform = InterpolateToNextAnimation(joint, jointTo);
			}
			else
			{
				joint->Update(mCurrentTime);
				nodeTransform = joint->GetLocalTransform();
			}
		}
		else
		{
			nodeTransform = node->transform;
		}
		glm::mat4 globalTransformation = parentTransform * nodeTransform;
		auto      boneInfoMap = mCurrentAnimation->GetBoneIDMap();
		if (boneInfoMap.find(nodeName) != boneInfoMap.end())
		{
			int       index = boneInfoMap[nodeName].id;
			glm::mat4 offset = boneInfoMap[nodeName].offset;
			mPose[index] = mCurrentAnimation->InvRootTransform() * globalTransformation * offset;

#ifdef EKDEBUG
			mLocalPose[index] = GetOwner().Transform().GetWorldMatrix() * mCurrentAnimation->InvRootTransform() * globalTransformation;
#endif
		}
		for (int i = 0; i < node->childrenCount; i++)
			AdvanceBones(&node->children[i], globalTransformation);
	}

#ifdef EKDEBUG
	void AnimationComponent::DebugDraw(Renderer::DebugRenderer& debugRenderer)
	{
		glm::vec3 rootPos = GetOwner().Transform().GetWorldMatrix()[3];

		std::function<void(const AssimpNodeData*, glm::vec3)> DrawChildren = [this, &DrawChildren, &debugRenderer](const AssimpNodeData* node,
		                                                                                                           glm::vec3             parentBonePos) -> void
		{
			auto boneInfoMap = mCurrentAnimation->GetBoneIDMap();

			for (int i = 0; i < node->childrenCount; i++)
			{
				const AssimpNodeData* kid = &node->children[i];
				if (boneInfoMap.find(kid->name) != boneInfoMap.end())
				{
					int       index = boneInfoMap[kid->name].id;
					glm::vec3 pos = mLocalPose[index][3];
					debugRenderer.DrawLine(parentBonePos, pos, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 3.0f);
					DrawChildren(&node->children[i], pos);
				}
			}
		};

		debugRenderer.DrawSphere(rootPos, 2.0f, glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));

		DrawChildren(&mCurrentAnimation->GetRootNode(), rootPos);
	}
#endif
} // namespace Eklavya
