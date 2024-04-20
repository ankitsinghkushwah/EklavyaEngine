#pragma once

#include <vector>
#include <string>
#include <glad/glad.h>
#include "IAsset.h"

namespace Eklavya::Asset {
class Cubemap : public IAsset {
public:
  Cubemap() = default;
  Cubemap(const std::string &assetName);
  ~Cubemap();
  void Create(const std::vector<std::string> &ordererdFaceNames);
  void BindToUnit(GLenum textureUnit);

private:
  GLuint m_ID;
};

} // namespace Eklavya::Asset
