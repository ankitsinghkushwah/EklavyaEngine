//
//  Framebuffer.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 16/06/2023.
//

#ifndef Framebuffer_hpp
#define Framebuffer_hpp

#include <glad/glad.h>

namespace Eklavya::Renderer {
class IFramebuffer {
public:
  IFramebuffer() {}

  virtual ~IFramebuffer() {}

  virtual bool Setup() = 0;
  virtual void Bind() = 0;
  virtual void Destroy() = 0;
  virtual void Unbind() = 0;

  virtual unsigned int ColorOutput() const { return 0; }

  virtual unsigned int DepthOutput() const { return 0; }

  inline unsigned int GetID() const { return mID; }

protected:
  GLuint mID;
};

class ShadowPassFB : public IFramebuffer {
public:
  ShadowPassFB(int depthPrecision, int width, int height);
  ~ShadowPassFB();

  bool Setup() override;
  void Bind() override;
  void Unbind() override;
  void Destroy() override;

  unsigned int DepthOutput() const override { return mDepthOutput; }

private:
  unsigned int mDepthOutput;
  int mDepthSize;
  int mWidth;
  int mHeight;
};

class GeneralFB : public IFramebuffer {
public:
  GeneralFB(int depthSize, int colorChannels, float width, float height);
  ~GeneralFB();

  bool Setup() override;
  void Bind() override;
  void Unbind() override;
  void Destroy() override;

  unsigned int ColorOutput() const override { return mColorOutput; }

private:
  unsigned int mColorOutput;
  unsigned int mDepthRBO;
  float mWidth, mHeight;
  int mDepthSize;
  int mChannels;
};
} // namespace Eklavya::Renderer

#endif /* Framebuffer_hpp */
