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
		glm::mat4 parentTransform;
		glm::mat4 sourceWorldTransform;
		glm::mat4 sourceLocalTransform;
		glm::mat4 finalTransform;
	};

	class AnimationIKSolver : public EkComponent
	{
	public:
		AnimationIKSolver(EkActor &owner, const EkScene &scene);

		~AnimationIKSolver() {}

		void Tick(float dt) override;

		glm::mat4 GetJointTransform(const std::string &name) const;

		bool ContainsJoint(const std::string &name) const;

		const JointData *GetJointData(const std::string &name) const;

		void SolveForLeg(std::array<JointData, 3> &legJoints, glm::vec3 targetPos, bool leftLeg);

		void Solve(const std::map<std::string, BoneInfo> &boneInfoMap,
		           const std::vector<glm::mat4> &boneWorldTransforms,
		           const std::vector<glm::mat4> &boneLocalTransforms,
		           const std::vector<glm::mat4> &parentTransforms);

	private:
		const std::string mLeftLegBoneName = "mixamorig_LeftLeg";
		const std::string mRightLegBoneName = "mixamorig_RightLeg";

		float mRayCastRange = 100.0f;

		std::array<std::array<JointData, 3>, 2> mLegsJointsData;

		glm::vec3 mLeftFootWorldPosition;
		glm::vec3 mRightFootWorldPosition;
		Eklavya::Physics::CastHitResult mLeftFootRayCastResult;
		Eklavya::Physics::CastHitResult mRightFootRayCastResult;

		const EkScene &mScene;


		void DebugDraw(Renderer::DebugRenderer &renderer) override;
	};
} // namespace Eklavya
#endif /* AninmationIKSolverComponent_hpp */
