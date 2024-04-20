//
//  Framebuffer.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 16/06/2023.
//

#include "Framebuffer.hpp"
#include <stdio.h>

using namespace Eklavya::Renderer;

// SHADOW PASS
ShadowPassFB::ShadowPassFB(int depthPrecision, int width, int height)
    : mDepthSize(depthPrecision), mWidth(width), mHeight(height) {}

ShadowPassFB::~ShadowPassFB() {}

bool ShadowPassFB::Setup() {
  glGenFramebuffers(1, &mID);
  glBindFramebuffer(GL_FRAMEBUFFER, mID);

  // Creating color attachemnt
  glGenTextures(1, &mDepthOutput);
  glBindTexture(GL_TEXTURE_2D, mDepthOutput);

  GLenum depthFormat = GL_DEPTH_COMPONENT24;
  if (16 == mDepthSize)
    depthFormat = GL_DEPTH_COMPONENT16;
  else if (32 >= mDepthSize)
    depthFormat = GL_DEPTH_COMPONENT32;

  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, mWidth, mHeight, 0,
               GL_DEPTH_COMPONENT, GL_FLOAT, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
                         mDepthOutput, 0);

  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR:: SHADOW PASS FRAMEBUFFER:: Framebuffer is not complete!");
    return false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return true;
}

void ShadowPassFB::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, mID); }

void ShadowPassFB::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void ShadowPassFB::Destroy() {
  glDeleteTextures(1, &mDepthOutput);
  glDeleteFramebuffers(1, &mID);
}

// FOR MAIN PASS

GeneralFB::GeneralFB(int depthSize, int colorChannels, float width,
                     float height)
    : mChannels(colorChannels), mDepthSize(depthSize), mWidth(width),
      mHeight(height) {}

GeneralFB::~GeneralFB() {}

bool GeneralFB::Setup() {
  if (mDepthSize != 24 && mDepthSize != 16 && mDepthSize != 32) {
    printf("Main Pass FB error : depth size is invalid");
    return false;
  }

  glGenFramebuffers(1, &mID);
  glBindFramebuffer(GL_FRAMEBUFFER, mID);

  // Creating color attachemnt
  glGenTextures(1, &mColorOutput);
  glBindTexture(GL_TEXTURE_2D, mColorOutput);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, mWidth, mHeight, 0, GL_RGB,
               GL_UNSIGNED_BYTE, 0);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                         mColorOutput, 0);

  // Creating depth attachment
  glGenRenderbuffers(1, &mDepthRBO);
  glBindRenderbuffer(GL_RENDERBUFFER, mDepthRBO);

  GLenum depthFormat = GL_DEPTH_COMPONENT24;
  if (16 == mDepthSize)
    depthFormat = GL_DEPTH_COMPONENT16;
  else if (32 == mDepthSize)
    depthFormat = GL_DEPTH_COMPONENT32;

  glRenderbufferStorage(GL_RENDERBUFFER, depthFormat, mWidth, mHeight);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                            GL_RENDERBUFFER, mDepthRBO);

  if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    printf("ERROR:: MAIN PASS FRAMEBUFFER:: Framebuffer is not complete!");
    return false;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  return true;
}

void GeneralFB::Bind() { glBindFramebuffer(GL_FRAMEBUFFER, mID); }

void GeneralFB::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void GeneralFB::Destroy() {
  glDeleteTextures(1, &mColorOutput);
  glDeleteRenderbuffers(1, &mDepthRBO);
  glDeleteFramebuffers(1, &mID);
}
