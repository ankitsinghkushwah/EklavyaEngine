#ifndef INC_ANIM_COMPONENT
#define INC_ANIM_COMPONENT
#include "EkComponent.hpp"
#include <glm/glm.hpp>
#include <map>
#include <vector>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include "../UserInputListener.h"
#include "AnimationData.h"

namespace Eklavya::Asset
{
  class Animation;
  class AssimpNodeData;
} // namespace Eklavya::Asset
class Bone;

namespace Eklavya
{
  class Animator : public EkComponent
  {
  public:
    Animator(ModelID modelId);
    ~Animator();
    void Init() override;
    void Tick(float dt) override;

    void PlayAnimation(std::shared_ptr<Asset::Animation> pAnimation);

    const std::vector<glm::mat4> &GetPoseTransforms() { return mPose; }

    ModelID                       GetModelID() { return mModelID; }

    // TEMP function
    void PlayAnimation(int index)
    {
      mNextAnimation = mAnimations[index];
      mIsSwitchingAnimation = true;
      mElapsedAnimTransition = 0;
    }

  private:
    float     GetScaleFactor(float lastTimeStamp, float nextTimeStamp);
    glm::mat4 InterpolateToNextAnimation(Bone *jointFrom, Bone *jointTo);
    glm::mat4 LerpPos(Bone *jointFrom, Bone *jointTo, float scaleFactor);
    glm::mat4 SlerpRot(Bone *jointFrom, Bone *jointTo, float scaleFactor);
    glm::mat4 LerpScale(Bone *jointFrom, Bone *jointTo, float scaleFactor);
    void      AdvanceBones(const Asset::AssimpNodeData *node,
                           glm::mat4                    parentTransform);

    ModelID   mModelID = -1;
    std::vector<glm::mat4>                         mPose;
    std::vector<glm::mat4>                         mLocalPose;
    std::shared_ptr<Asset::Animation>              mCurrentAnimation;
    std::shared_ptr<Asset::Animation>              mNextAnimation;
    std::vector<std::shared_ptr<Asset::Animation>> mAnimations;
    float                                          mCurrentTime = 0.0f;
    float                                          mDeltaTime;
    float                                          mElapsedAnimTransition;
    float                                          mMaxAnimTransitionDuration;
    bool                                           mIsSwitchingAnimation;
    float                                          mTransitionSpeed;

#ifdef EKDEBUG
  private:
   
    virtual void DebugDraw(Renderer::DebugRenderer &debugRenderer);
#endif
  };

  DECLARE_COMPONENT_ESSENTIALS(Animator, ANIMATOR);
} // namespace Eklavya
#endif
