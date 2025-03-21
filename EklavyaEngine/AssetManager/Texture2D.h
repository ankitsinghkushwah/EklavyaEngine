#ifndef INC_TEXTURE2D_H_
#define INC_TEXTURE2D_H_

#include "IAsset.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>

namespace Eklavya::Asset
{
  class Texture2D : public IAsset
  {
  public:
    Texture2D();

    Texture2D(const std::string &assetName);

    int CreateTexture(std::string path, bool repeat = false);

    int CreateTextureFromBuffer(unsigned int bufferSize, void *imageData);

    ~Texture2D();

    void BindToUnit(uint32_t textureUnit);

    void Unbind();

    inline GLuint GetID() const { return mID; }

    inline glm::vec2 GetSize() const { return mSize; }

    inline GLenum GetFormat() const { return mFormat; }

    inline unsigned char *GetBits() const { return mImage; }

  protected:
    GLuint mID = 0;
    glm::vec2 mSize;
    GLenum mFormat;
    unsigned char *mImage = nullptr;
    uint32_t mBoundToUnit = 0;
  };
} // namespace Eklavya::Asset

#endif
