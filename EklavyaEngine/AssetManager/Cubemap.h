#ifndef INC_CUBEMAP_H
#define INC_CUBEMAP_H

#include "IAsset.h"
#include <glad/glad.h>
#include <string>
#include <vector>

namespace Eklavya::Asset
{
  class Cubemap : public IAsset
  {
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
#endif
