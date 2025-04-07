#ifndef _INC_GL_RENDERER_H_
#define _INC_GL_RENDERER_H_

#include "RenderPass.hpp"
#include <AssetManager/ShaderProgram.h>
#include <Components/RenderComponent.hpp>
#include <GLFWGame.h>
#include <Renderer/Material.h>
#include <array>
#include <unordered_map>

#ifdef EKDEBUG
#include <Renderer/DebugRenderer.hpp>
#endif

namespace Eklavya
{
  class EkScene;
  class RenderComponent;
} // namespace Eklavya

namespace Eklavya::Renderer
{
  using GROUP_MATERIALS = std::array<SHARED_MATERIAL, ERenderGroup::RG_MAX>;

  class DebugRenderer;

  class GLRenderer
  {
    friend MainPass;
    friend ShadowMapPass;

  public:
    GLRenderer(const GLWindowContext &context);

    void LoadBuiltInMaterials();

    void Init();

    ~GLRenderer();

    void AddMaterialToState(ERenderGroup group, SHARED_MATERIAL material);

    template<typename MATERIAL_TYPE>
    std::shared_ptr<MATERIAL_TYPE> GetMaterialForGroup(ERenderGroup group) const
    {
      return std::static_pointer_cast<MATERIAL_TYPE>(mGroupMaterials[group]);
    }

    void Render(EkScene &scene);

    void AddActor(const RenderComponent &renderComponent);

    void ImGuiProc();

  private:
    void RenderInternal(const std::vector<const RenderComponent *> &actors,
                        SHARED_SHADER &shader,
                        SHARED_MATERIAL material = nullptr);

    void PostPassesResult();

    void SetShaderDataForActor(const RenderComponent &renderComponent,
                               SHARED_SHADER &shader);

    GROUP_MATERIALS mGroupMaterials;

    Eklavya::Asset::SHARED_SHADER mMainOutputShader = nullptr;
    std::array<std::unique_ptr<IRenderPass>, ERenderPass::RP_MAX> mRenderPasses;
    GLuint mVAO;
    const GLWindowContext &mContext;
    glm::mat4 mOutputProjection;
    glm::mat4 mOutputModel;

  public:
    DebugRenderer &GetDebugRenderer() { return mDebugRenderer; }

  private:
    void DebugDraw(EkScene &scene);

    DebugRenderer &mDebugRenderer;
  };
} // namespace Eklavya::Renderer

#endif
