#ifndef INC_ANIMATION
#define INC_ANIMATION

#include <vector>
#include <map>
#include <assimp/scene.h>
#include <AssetManager/Bone.h>
#include <functional>
#include <AssetManager/AnimationData.h>
#include "IAsset.h"

namespace Eklavya::Asset
{
  struct AssimpNodeData
	{
		glm::mat4                   transform;
		std::string                 name;
		int                         childrenCount;
		std::vector<AssimpNodeData> children;
	};

	class Animation : public IAsset
	{
	  public:
		Animation() = default;
		Animation(const std::string& assetName, int modelID, const aiAnimation* animation, const aiScene* scene);
		~Animation();
		Bone* FindJoint(const std::string& name);

		// getters
		inline glm::mat4 RootTransform()
		{
			return mRootTransform;
		}

		inline glm::mat4 InvRootTransform()
		{
			return mInvRootTransform;
		}

		inline float GetTicksPerSecond()
		{
			return m_TicksPerSecond;
		}

		inline float GetDuration()
		{
			return m_Duration;
		}

		inline float GetRealDuration()
		{
			return m_Duration / m_TicksPerSecond;
		}

		inline const AssimpNodeData& GetRootNode()
		{
			return m_RootNode;
		}

		inline const std::map<std::string, BoneInfo>& GetBoneIDMap()
		{
			return m_BoneInfoMap;
		}

	  private:
		void SetupJoints(const aiAnimation* animation, const aiScene& scene);
		void ReadHeirarchyData(AssimpNodeData& dest, const aiNode& src);

		float                           m_Duration;
		int                             m_TicksPerSecond;
		glm::mat4                       mRootTransform;
		glm::mat4                       mInvRootTransform;
		std::vector<Bone>               m_Joints;
		AssimpNodeData                  m_RootNode;
		std::map<std::string, BoneInfo> m_BoneInfoMap;
		int                             m_ModelID;
	};
} // namespace Eklavya::Asset

#endif
