//
//  RenderPass.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 16/06/2023.
//

#include "RenderPass.hpp"
#include "GLRenderer.h"
#include "EkScene.h"
#include <glm/gtc/matrix_transform.hpp>
#include <stdio.h>
#include "FreeLookCamera.h"
#include <memory.h>
#include "AssetManager.h"
#include <Random.h>
#include "DebugRenderer.hpp"

using namespace Eklavya::Renderer;

glm::vec3 lightPosition(50.0f, 50.0f, -50.0f);
glm::vec3 lightColor(30.0f);

// BASE==================

IRenderPass::IRenderPass(ERenderPass type) : mType(type) {}

IRenderPass::~IRenderPass() {}

// SHADOW ========================

ShadowMapPass::ShadowMapPass(int depthPrecision, int width, int height)
    : IRenderPass(ERenderPass::SHADOW_PASS), mDepthPrecision(depthPrecision),
      mWidth(width), mHeight(height) {}

ShadowMapPass::~ShadowMapPass() {}

bool ShadowMapPass::Init() {
  mShader = AssetManager::GetInstance().GetAsset<Eklavya::Asset::ShaderProgram>(
      "shadow_map");
  mFramebuffer =
      std::make_unique<ShadowPassFB>(mDepthPrecision, mWidth, mHeight);
  mFramebuffer->Setup();
  return false;
}

void ShadowMapPass::TryAddingActor(
    const SHARED_RENDERCOMPONENT &renderComponent) {
  if (renderComponent->mProjectsShadow)
    mActors.push_back(renderComponent);
}

void ShadowMapPass::PreRun() {
  mFramebuffer->Bind();
  glViewport(0, 0, mWidth, mHeight);
  glClear(GL_DEPTH_BUFFER_BIT);
}

void ShadowMapPass::Run(GLRenderer &renderer, const EkScene &scene) {
  glCullFace(GL_FRONT);

  float far_plane = 500.0f;
  float extents = 1500;
  glm::mat4 projection =
      glm::ortho(-extents, extents, -extents, extents, -far_plane, far_plane);

  glm::mat4 view = glm::lookAt(lightPosition, glm::vec3(0.0f, 0.0f, 0.0f),
                               glm::vec3(0.0f, 1.0f, 0.0f));
  mLightPVMatrix = projection * view;
  mShader->Use();
  mShader->SetMat4("lightProjectionView", mLightPVMatrix);
  renderer.RenderInternal(mActors, mShader);
  mActors.clear();

  glCullFace(GL_BACK);
}

void ShadowMapPass::PostRun() { mFramebuffer->Unbind(); }

#ifdef EKDEBUG
void ShadowMapPass::DebugDraw(DebugRenderer &dr) {}
#endif

// MAIN ========================

MainPass::MainPass(int depth, int colorChannels, int width, int height)
    : IRenderPass(ERenderPass::MAIN_PASS), mDepthPrecision(depth),
      mWidth(width), mHeight(height) {}

MainPass::~MainPass() { mFramebuffer->Destroy(); }

bool MainPass::Init() {
  mFramebuffer =
      std::make_unique<GeneralFB>(mDepthPrecision, 4, mWidth, mHeight);
  mFramebuffer->Setup();
  return true;
}

void MainPass::TryAddingActor(const SHARED_RENDERCOMPONENT &renderComponent) {
  if (renderComponent->mInsideFrustum ||
      renderComponent->mRenderGroup == ERenderGroup::SKYBOX) {
    mActorsMap[renderComponent->mRenderGroup].push_back(renderComponent);
  }
}

void MainPass::PreRun() {
  mFramebuffer->Bind();
  glViewport(0, 0, mWidth, mHeight);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void MainPass::Run(GLRenderer &renderer, const EkScene &scene) {
  glm::mat4 projection = scene.CurrentCamera()->GetProjection();
  glm::mat4 view = scene.CurrentCamera()->GetView();
  glm::vec3 viewSpacePos = view * glm::vec4(lightPosition, 1.0f);

  const ShadowMapPass *shadowPass =
      (ShadowMapPass *)renderer.mRenderPasses[0].get();
  glm::mat4 lightMatrix = shadowPass->mLightPVMatrix;
  unsigned int shadowMap = shadowPass->mFramebuffer->DepthOutput();

  for (uint32_t group = 0; group < ERenderGroup::RG_MAX; ++group) {
    SHARED_MATERIAL material = renderer.mGroupMaterials[group];
    auto &actorsVec = mActorsMap[group];

    material->Activate();

    SHARED_SHADER shader = material->m_Shader;

    shader->SetMat4("projection", projection);
    shader->SetMat4("view", view);

    shader->SetMat4("lightSpaceMatrix", lightMatrix);
    shader->SetVec3("light.position", viewSpacePos);
    shader->SetVec3("light.color", lightColor);
    shader->SetInt("shadowPassOutput", 8);
    glActiveTexture(GL_TEXTURE0 + 8);
    glBindTexture(GL_TEXTURE_2D, shadowMap);

    renderer.RenderInternal(actorsVec, shader, material);

    actorsVec.clear();
    material->Deactivate();
  }
}

void MainPass::PostRun() { mFramebuffer->Unbind(); }

#ifdef EKDEBUG
void MainPass::DebugDraw(DebugRenderer &dR) {}
#endif
