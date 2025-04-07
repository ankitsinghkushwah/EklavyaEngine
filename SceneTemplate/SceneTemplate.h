//
// Created by Ankit Singh Kushwah on 16/03/2025.
//

#ifndef SCENETEMPLATE_H
#define SCENETEMPLATE_H

#include <MainEntryScene.hpp>
#include <Renderer/Material.h>
#include <AssetManager/MaterialInfo.h>
#include "Scene/SceneHelper.hpp"

namespace Eklavya
{
    class SceneTemplate final : public MainEntryScene
    {
    public:
        SceneTemplate(class Director &pDirector);

        ~SceneTemplate() override;

    private:
        void PreloadTextures();

        Asset::MaterialInfo LoadMaterialInfo(const std::string &file, std::string ext = "png");

        void CreateStage();

        void OnMouseAction(int key, int action) override;

        void OnKeyAction(int key, int action) override;

        void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;

        void ImGuiProc() override;
    };
} // namespace Eklavya

#endif
