#include "Animation.h"
#include <glm/gtc/type_ptr.hpp>
#include "../Helpers.h"
#include "AnimationData.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Eklavya::Asset
{
	Animation::Animation(const std::string& assetName, int modelID, const aiAnimation* animation, const aiScene* scene)
	    : IAsset(EType::ANIMATION, assetName)
	    , m_ModelID(modelID)
	{
		m_Duration = animation->mDuration;
		m_TicksPerSecond = animation->mTicksPerSecond;
		aiMatrix4x4 globalTransformation = scene->mRootNode->mTransformation;
		mRootTransform = GetGLMMat(globalTransformation);
		mInvRootTransform = GetGLMMat(globalTransformation.Inverse());
		assert(scene && scene->mRootNode);
		ReadHeirarchyData(m_RootNode, *scene->mRootNode);
		SetupJoints(animation, *scene);
	}

	Animation::~Animation()
	{
	}

	void Animation::SetupJoints(const aiAnimation* animation, const aiScene& scene)
	{
		int   size = animation->mNumChannels;
		auto& modelDataMap = ModelsBoneData::s_BonesDataMap[m_ModelID];
		auto& boneInfoMap = modelDataMap.m_BoneInfoMap;
		int&  boneCount = modelDataMap.m_BoneCount;
		for (int i = 0; i < size; i++)
		{
			auto        channel = animation->mChannels[i];
			std::string boneName = channel->mNodeName.data;
			if (boneInfoMap.find(boneName) == boneInfoMap.end())
			{
				boneInfoMap[boneName].id = boneCount;
				//     boneInfoMap[boneName].offset = glm::mat4(1.0f);
				boneCount++;
			}
			m_Joints.push_back(Bone(channel->mNodeName.data, boneInfoMap[channel->mNodeName.data].id, channel));
		}
		m_BoneInfoMap = boneInfoMap;
	}

	void Animation::ReadHeirarchyData(AssimpNodeData& dest, const aiNode& src)
	{
		dest.transform = GetGLMMat(src.mTransformation);
		dest.name = src.mName.data;
		dest.childrenCount = src.mNumChildren;
		for (int i = 0; i < src.mNumChildren; i++)
		{
			AssimpNodeData newData;
			const aiNode*  assimpNode = src.mChildren[i];
			ReadHeirarchyData(newData, *assimpNode);
			dest.children.push_back(newData);
		}
	}

	Bone* Animation::FindJoint(const std::string& name)
	{
		auto iter = std::find_if(m_Joints.begin(), m_Joints.end(), [&](const Bone& joint) { return joint.GetJointName() == name; });
		if (iter == m_Joints.end())
			return nullptr;
		else
			return &(*iter);
	}
} // namespace Eklavya::Asset
