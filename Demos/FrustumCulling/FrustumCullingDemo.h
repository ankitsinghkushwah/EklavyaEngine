//
// Created by Ankit Singh Kushwah on 16/03/2025.
//

#ifndef INCFRUSTUMCULLINGDEMO_H
#define INCFRUSTUMCULLINGDEMO_H

#include <MainEntryScene.hpp>
#include <Renderer/Material.h>
#include <AssetManager/MaterialInfo.h>
#include "Scene/SceneHelper.hpp"

namespace Eklavya
{
    class FrustumCullingDemo final : public MainEntryScene
    {
    public:
        FrustumCullingDemo(Director &pDirector);

        ~FrustumCullingDemo() override;

    private:
        void PreloadTextures();

        Asset::MaterialInfo LoadMaterialInfo(const std::string &file, std::string ext = "png");

        void CreateStage();

        void OnMouseAction(int key, int action) override;

        void OnKeyAction(int key, int action) override;

#ifdef EKDEBUG
        void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;

        void ImGuiProc() override;
#endif
    };
} // namespace Eklavya

#endif
