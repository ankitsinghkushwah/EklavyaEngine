//
// Created by Ankit Singh Kushwah on 16/03/2025.
//

#ifndef IncShadowsDemo_h
#define IncShadowsDemo_h

#include <MainEntryScene.hpp>
#include <AssetManager/MaterialInfo.h>
#include "Scene/SceneHelper.hpp"

namespace Eklavya
{
    class ShadowsDemo : public MainEntryScene
    {
    public:
        ShadowsDemo(Director &pDirector);

        ~ShadowsDemo() override;

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
}

#endif
