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
#include <Components/EkComponent.hpp>
#include <EkPhysics/PhysicsWorld.h>
#include <array>
#include <AssetManager/AnimationData.h>

#include "AssetManager/GLModel.hpp"
#include "assimp/scene.h"

namespace Eklavya
{
	class AnimationComponent;
}

namespace Eklavya::Asset
{
	class Animation;
	class AssimpNodeData;
} // namespace Eklavya::Asset
class Bone;


namespace Eklavya
{
	class EkScene;


	namespace Renderer
	{
		class DebugRenderer;
	}

	struct JointData
	{
		std::string boneName;
		glm::vec3 position;
		glm::mat4 parentTransform = glm::mat4(1.0f);
		glm::mat4 worldTransform = glm::mat4(1.0f);
		glm::mat4 baseWorldTransform = glm::mat4(1.0f);
		glm::mat4 finalTransform = glm::mat4(1.0f);
		bool overrideTransform = false;
		float boneAngle = 0.0f;
	};

	class AnimationIKSolver : public EkComponent
	{
	public:
		AnimationIKSolver(EkActor &owner, const EkScene &scene);

		~AnimationIKSolver() {}


		void Tick(float dt) override;

		void AdvanceBones(const Asset::ModelNode &node,
		                  glm::mat4 globalTransform, bool found);

		glm::mat4 GetJointTransform(const std::string &name) const;

		bool ContainsJoint(const std::string &name) const;

		const JointData *GetJointData(const std::string &name) const;

		void Solve2DForLeg(std::array<JointData, 3> &legJoints, glm::vec3 targetPos, bool leftLeg);

		void AdjustFoot();

		void Solve(const std::map<std::string, BoneInfo> &boneInfoMap);

		const std::vector<glm::mat4> &GetPoseTransforms() { return mPose; }

		int mModelID = 0;

	private:
		const std::string mLeftLegBoneName = "mixamorig_LeftUpLeg";
		const std::string mRightLegBoneName = "mixamorig_RightUpLeg";
		const std::string mHipBone = "mixamorig_Spine";

		float mRayCastRange = 100.0f;

		std::array<std::array<JointData, 3>, 2> mLegsJointsData;

		glm::vec3 mLeftFootWorldPosition;
		glm::vec3 mRightFootWorldPosition;
		Eklavya::Physics::CastHitResult mLeftFootRayCastResult;
		Eklavya::Physics::CastHitResult mRightFootRayCastResult;

		std::vector<glm::mat4> mPose;
		std::vector<glm::mat4> mBoneWorldPoses;
		std::vector<glm::mat4> mBasePose;

		const EkScene &mScene;

		glm::mat4 mRootInverse;
		glm::mat4 mRoot;
		Asset::GLModel *mModel = nullptr;

		glm::vec3 mLeftLegTargetPos;

		AnimationComponent *mAnimationComponent = nullptr;


		void ImGuiProc() override;

		void DebugDraw(Renderer::DebugRenderer &renderer) override;
	};
} // namespace Eklavya
#endif /* AninmationIKSolverComponent_hpp */
