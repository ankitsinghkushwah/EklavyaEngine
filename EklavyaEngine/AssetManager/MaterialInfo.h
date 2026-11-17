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
  // Used when there is no base map. Given in sRGB, like a colour picker value.
  glm::vec3 mBaseColor = glm::vec3(0.8f);
  // Blinn-Phong highlight: strength is 0..1, shininess is the Phong exponent.
  float mSpecularStrength = 0.25f;
  float mShininess = 32.0f;
  SHARED_TEXTURE mBaseMap = nullptr;
  SHARED_TEXTURE mNormalMap = nullptr;
};

} // namespace Eklavya::Asset

#endif /* MaterialInfo_h */
