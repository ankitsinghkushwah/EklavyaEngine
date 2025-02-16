#ifndef ANIM_DATA_H_
#define ANIM_DATA_H_

/*

        Number of bones in Animation and Number of bones in Model might differ,
        so to make sure that final animation transform matrix have same number
   of joint transforms as in the required animation, I kept "BonesData" Global.

        New bones are added by animation if they were missing while loading
   model data. :-)

*/

#include <glm/glm.hpp>
#include <map>
#include <string>

constexpr std::uint32_t MAX_BONES_SUPPORTED = 100;

using ModelID = int;

struct BoneInfo
{
	/*
          For uniquely indentifying the bone and
          for indexing bone transformation in shaders
  */
	int id;
	/*
          map from bone name to offset matrix.
          offset matrix transforms bone from bone space to local space
  */
	glm::mat4 offset;
};

struct BonesData
{
	std::map<std::string, BoneInfo> m_BoneInfoMap;
	int                             m_BoneCount;
};

struct ModelsBoneData
{
	static std::map<ModelID, BonesData> s_BonesDataMap;
};

#endif
