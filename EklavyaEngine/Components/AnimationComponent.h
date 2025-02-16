#ifndef INC_ANIM_COMPONENT
#define INC_ANIM_COMPONENT
#include "../UserInputListener.h"
#include <AssetManager/AnimationData.h>
#include <Components/EkComponent.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <map>
#include <vector>

namespace Eklavya::Asset
{
	class Animation;
	class AssimpNodeData;
} // namespace Eklavya::Asset
class Bone;

namespace Eklavya
{
	class EkActor;

	class AnimationComponent : public EkComponent
	{
	  public:
		AnimationComponent(EkActor& owner, ModelID modelId);
		~AnimationComponent();
		void Tick(float dt) override;

		std::shared_ptr<Asset::Animation> GetCurrentAnimation() const { return mCurrentAnimation; }
		void                              PlayAnimation(std::shared_ptr<Asset::Animation> pAnimation, bool smoothTransition = false);

		const std::vector<glm::mat4>& GetPoseTransforms() { return mPose; }

		ModelID GetModelID() { return mModelID; }

		// TEMP function
		void PlayAnimation(int index)
		{
			mNextAnimation = mAnimations[index];
			mIsSwitchingAnimation = true;
			mElapsedAnimTransition = 0;
		}

		glm::mat4 GetBoneTransform(const std::string& boneName) const;

	  private:
		float     GetScaleFactor(float lastTimeStamp, float nextTimeStamp);
		glm::mat4 InterpolateToNextAnimation(Bone* jointFrom, Bone* jointTo);
		glm::mat4 LerpPos(Bone* jointFrom, Bone* jointTo, float scaleFactor);
		glm::mat4 SlerpRot(Bone* jointFrom, Bone* jointTo, float scaleFactor);
		glm::mat4 LerpScale(Bone* jointFrom, Bone* jointTo, float scaleFactor);
		void      AdvanceBones(const Asset::AssimpNodeData* node, glm::mat4 parentTransform);
		void      SolveIK(const Asset::AssimpNodeData* node, glm::mat4 parentTransform);

		ModelID                                        mModelID = -1;
		std::vector<glm::mat4>                         mPose;
		std::vector<glm::mat4>                         mBoneWorldPoses;
		std::vector<glm::mat4>                         mBoneLocalPoses;
		std::vector<glm::mat4>                         mParentTransforms;
		std::shared_ptr<Asset::Animation>              mCurrentAnimation;
		std::shared_ptr<Asset::Animation>              mNextAnimation;
		std::vector<std::shared_ptr<Asset::Animation>> mAnimations;
		float                                          mCurrentTime = 0.0f;
		float                                          mDeltaTime = 0.0f;
		float                                          mElapsedAnimTransition = 0.0f;
		float                                          mMaxAnimTransitionDuration = 0.0f;
		bool                                           mIsSwitchingAnimation = false;
		float                                          mTransitionSpeed = 0.0f;

#ifdef EKDEBUG
	  private:
		virtual void DebugDraw(Renderer::DebugRenderer& debugRenderer);
#endif
	};
} // namespace Eklavya
#endif
