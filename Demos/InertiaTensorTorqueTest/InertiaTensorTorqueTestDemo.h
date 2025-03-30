//
// Created by Ankit Singh Kushwah on 16/03/2025.
//

#ifndef INCINERTIATENSORTORQUETESTDEMO_H
#define INCINERTIATENSORTORQUETESTDEMO_H

#include <MainEntryScene.hpp>
#include <Renderer/Material.h>
#include <AssetManager/MaterialInfo.h>
#include "Scene/SceneHelper.hpp"

namespace Eklavya
{
    class InertiaTensorTorqueTestDemo final : public MainEntryScene
    {
    public:
        InertiaTensorTorqueTestDemo(class Director &pDirector);

        ~InertiaTensorTorqueTestDemo() override;

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

    private:
        Physics::EkBody *mBoxBody = nullptr;
        glm::vec3 mRayStart;
        glm::vec3 mRayDirection;
        float mRayRange = 0.0f;
        Physics::CastHitResult mLastCastHitResult;
        glm::vec3 mBoxScale;
        float mBoxMass = 100.0f;
        int mBoxIDCounter = 0;
        bool mForceApplied = false;
        std::string mForceAppliedText;
        float mImpulseForce = 100.0f;
        int mImpulseTargetID = -1; // 0 = left , 1 = middle , 2 = right
    };
} // namespace Eklavya

#endif
