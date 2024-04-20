#include "Material.h"
#include "Texture2D.h"
#include "ShaderProgram.h"
#include "Cubemap.h"

namespace Eklavya::Renderer {
MATERIAL_ID IMaterial::s_IdCounter = 0;

IMaterial::IMaterial(SHARED_SHADER shader, ERenderGroup state)
    : m_Shader(shader), m_State(state), mID(s_IdCounter++) {}

IMaterial::~IMaterial() {}

void IMaterial::Activate() {}

void IMaterial::Deactivate() {}

void IMaterial::Tick(float dt) {}

// PBRMaterial

PBRMaterial::PBRMaterial(SHARED_SHADER shader)
    : IMaterial(shader, ERenderGroup::ACTOR) {}

void PBRMaterial::Activate() {
  m_Shader->Use();

  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);
  glDisable(GL_CULL_FACE);
  //   glFrontFace(GL_CCW);

  m_Active = true;
}

void PBRMaterial::SetMaterialInfo(const Asset::MaterialInfo &info) {

  m_Shader->SetVec3("material.color", info.mBaseColor);
  m_Shader->SetFloat("material.roughness", info.mRoughness);
  m_Shader->SetFloat("material.metallic", info.mMetallic);
  m_Shader->SetFloat("material.opacity", info.mOpacity);
  m_Shader->SetInt("material.tiling", info.mTiling);

  uint32_t texIndex = 0;

  m_Shader->SetInt("material.bAlbedo", info.mBaseMap != nullptr);
  m_Shader->SetInt("material.bNormal", info.mNormalMap != nullptr);
  m_Shader->SetInt("material.bAO", info.mAOMap != nullptr);
  m_Shader->SetInt("material.bRoughness", info.mRoughMap != nullptr);
  m_Shader->SetInt("material.bMetallic", info.mMetalMap != nullptr);
  m_Shader->SetInt("bCubemap", info.mCubemap != nullptr);

  if (info.mBaseMap != nullptr) {
    m_Shader->SetInt("material.albedoMap", texIndex);
    info.mBaseMap->BindToUnit(texIndex);
  }

  texIndex++;

  if (info.mNormalMap != nullptr) {
    m_Shader->SetInt("material.normalMap", texIndex);
    info.mNormalMap->BindToUnit(texIndex);
  }

  texIndex++;

  if (info.mRoughMap != nullptr) {
    m_Shader->SetInt("material.roughnessMap", texIndex);
    info.mRoughMap->BindToUnit(texIndex);
  } else {
    m_Shader->SetFloat("material.roughness", info.mRoughness);
  }

  texIndex++;
  if (info.mMetalMap != nullptr) {
    m_Shader->SetInt("material.metallicMap", texIndex);
    info.mMetalMap->BindToUnit(texIndex);
  } else {
    m_Shader->SetFloat("material.metallic", info.mMetallic);
  }

  texIndex++;

  if (info.mAOMap != nullptr) {
    m_Shader->SetInt("material.aoMap", texIndex);
    info.mAOMap->BindToUnit(texIndex);
  }

  texIndex++;

  if (info.mCubemap != nullptr) {
    m_Shader->SetInt("cubemap", texIndex);
    info.mCubemap->BindToUnit(texIndex);
  }
}

void PBRMaterial::Deactivate() { m_Active = false; }

// SkyBox

SkyboxMaterial::SkyboxMaterial(SHARED_SHADER shader)
    : IMaterial(shader, ERenderGroup::SKYBOX) {}

void SkyboxMaterial::Activate() {
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);
  glEnable(GL_CULL_FACE);
  glFrontFace(GL_CW);
  m_Shader->Use();

  if (mCubemap != nullptr) {
    glActiveTexture(GL_TEXTURE0);
    m_Shader->SetInt("cubemap", 0);
    mCubemap->BindToUnit(0);
  }

  m_Active = true;
}

void SkyboxMaterial::Deactivate() { m_Active = false; }

} // namespace Eklavya::Renderer
