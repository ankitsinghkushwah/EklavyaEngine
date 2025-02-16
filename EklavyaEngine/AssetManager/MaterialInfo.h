//
//  MaterialInfo.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 17/06/2023.
//

#ifndef MaterialInfo_h
#define MaterialInfo_h

#include "Asset.h"
#include <glm/glm.hpp>
#include <string>

namespace Eklavya::Asset {

struct MaterialInfo {
  std::string mMeshName;
  uint32_t mTiling = 1;
  float mOpacity = 1.0f;
  float mMetallic = 0.5;
  float mRoughness = 0.5;
  glm::vec3 mBaseColor;
  SHARED_TEXTURE mBaseMap = nullptr;
  SHARED_TEXTURE mNormalMap = nullptr;
  SHARED_TEXTURE mAOMap = nullptr;
  SHARED_TEXTURE mRoughMap = nullptr;
  SHARED_TEXTURE mMetalMap = nullptr;
  SHARED_CUBEMAP mCubemap = nullptr;
};

} // namespace Eklavya::Asset

#endif /* MaterialInfo_h */
