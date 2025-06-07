//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "imgui/imgui.h"
#include <AssetManager/AssetManager.h>
#include <Components/RenderComponent.hpp>
#include <Director.hpp>
#include <EkPhysics/Collider.h>
#include <EkPhysics/EkBody.h>
#include <MainEntryScene.hpp>
#include <Random.h>
#include <Renderer/GLRenderer.h>
#include <Renderer/Material.h>
#include <Scene/EkActor.h>
#include <Scene/SceneHelper.hpp>

#include "FrustumCulling/FrustumCullingDemo.h"
#include "Shadows/ShadowsDemo.h"
#include "SkeletalAnimation/SkeletalAnimationDemo.hpp"
#include "SkeletalAnimationWithIK/SkeletalAnimationWithIK.hpp"
#include "StackOfBoxes/StackOfBoxesDemo.hpp"
#include "VehiclePhysicsDemo/VehiclePhysicsDemo.hpp"

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	enum MatIndex
	{
		GOLD,
		RUBBER,
		CRATE_SCIFI,
		CRATE_WOOD,
		PLASTIC
	};

	UniqueActor MainEntryScene::CreateBox(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate,
	                                      Asset::MaterialInfo matInfo)
	{
		UniqueActor floor = std::make_unique<EkActor>();
		floor->Transform().SetScale(scale);
		floor->Transform().SetPosition(pos);
		RenderComponent *render = floor->EmplaceComponent<RenderComponent>(EMeshType::CUBE);
		render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
		render->GetMesh().mMaterialInfo = matInfo;

		return floor;
	}

	UniqueActor MainEntryScene::CreateSphere(glm::vec3 pos, float radius, Asset::MaterialInfo matInfo)
	{
		UniqueActor sphere = std::make_unique<EkActor>();
		sphere->Transform().SetScale(radius);
		sphere->Transform().SetPosition(pos);
		auto render = sphere->EmplaceComponent<RenderComponent>(EMeshType::SPHERE);
		render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
		render->GetMesh().mMaterialInfo = matInfo;

		return sphere;
	}

	EkActor *MainEntryScene::AddBox(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate, float mass,
	                                Asset::MaterialInfo matInfo, uint32_t flag)
	{
		UniqueActor floor = std::make_unique<EkActor>();
		floor->Transform().SetScale(scale);
		floor->Transform().SetPosition(pos);
		RenderComponent *render = floor->EmplaceComponent<RenderComponent>(EMeshType::CUBE);
		render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
		render->GetMesh().mMaterialInfo = matInfo;

		auto collider = floor->EmplaceComponent<Physics::BoxColliderComponent>();

		collider->SetHalfSize(floor->Transform().GetScale());
		collider->SetGroupIndex(flag);

		auto body = floor->EmplaceComponent<Physics::EkBody>();
		body->SetMass(mass);
		body->SetPosition(pos);
		body->SetRotation(rotate);

		EkActor *actor = floor.get();
		AddActor(floor);

		return actor;
	}

	EkActor *MainEntryScene::AddSphere(glm::vec3 pos, float radius, float mass, Asset::MaterialInfo matInfo,
	                                   uint32_t flag)
	{
		UniqueActor sphere = std::make_unique<EkActor>();
		sphere->Transform().SetScale(radius);
		sphere->Transform().SetPosition(pos);
		auto render = sphere->EmplaceComponent<RenderComponent>(EMeshType::SPHERE);
		render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
		render->GetMesh().mMaterialInfo = matInfo;


		auto collider = sphere->EmplaceComponent<Physics::SphereColliderComponent>();
		collider->SetRadius(radius);
		collider->SetGroupIndex(flag);

		auto body = sphere->EmplaceComponent<Physics::EkBody>();
		body->SetMass(mass);
		body->SetPosition(pos);
		body->SetRestitution(.4f);

		EkActor *actor = sphere.get();

		AddActor(sphere);

		return actor;
	}

	MainEntryScene::MainEntryScene(Director &pDirector) :
		EkScene(pDirector)
	{
		GetRenderer().GetMaterialForGroup<SkyboxMaterial>(Renderer::ERenderGroup::SKYBOX)->mCubemap =
				AssetManager::GetInstance().LoadCubemap("Day", "png");

		UniqueActor sky = std::make_unique<EkActor>();
		RenderComponent *render = sky->EmplaceComponent<RenderComponent>(EMeshType::CUBE);
		render->mProjectsShadow = false;
		render->mRenderGroup = Renderer::ERenderGroup::SKYBOX;
		AddActor(sky);

		mDirector.HideMouse(false);
		CurrentCamera()->SetEnabled(false);
	}

	MainEntryScene::~MainEntryScene() {}

	void MainEntryScene::ImGuiProc()
	{
		EkScene::ImGuiProc();

		static std::vector<const char *> demos = {
			"Stack Of Boxes", "Vehicle Physics", "Skeletal Animation",
			"IK (inverse kinematics)", "Shadows", "Frustum Culling"
		};

		static bool isLoading = false; // Track loading state
		static int loadingDemoIndex = -1; // Which demo is loading
		static int loadingCounter = 0;

		int windowWidth = mDirector.GetCurrentContext()->GetWidth() * 0.5f;
		int windowHeight = mDirector.GetCurrentContext()->GetHeight() * 0.5f;
		ImVec2 windowSize(windowWidth, windowHeight);
		ImVec2 windowPos(windowWidth * 0.3f, windowHeight * 0.2f);

		ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);

		ImVec4 bgColor = ImVec4(0.90f, 0.69f, 0.67f, 0.6f);
		ImVec4 btnColor = ImVec4(0.50f, 0.74f, 0.82f, 1.0f);
		ImVec4 txtColor = ImVec4(0.31f, 0.20f, 0.18f, 1.0f);
		ImVec4 titleBg = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

		ImGui::PushStyleColor(ImGuiCol_WindowBg, bgColor);
		ImGui::PushStyleColor(ImGuiCol_Button, btnColor);
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
		                      ImVec4(btnColor.x + 0.1f, btnColor.y + 0.1f, btnColor.z + 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive,
		                      ImVec4(btnColor.x - 0.1f, btnColor.y - 0.1f, btnColor.z - 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_Text, txtColor);
		ImGui::PushStyleColor(ImGuiCol_TitleBg, titleBg);
		ImGui::PushStyleColor(ImGuiCol_TitleBgActive, titleBg);
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);

		ImGui::Begin("DEMOS", nullptr);

		ImGui::BeginChild("ButtonList", ImVec2(0, 0), false, ImGuiWindowFlags_AlwaysVerticalScrollbar);
		ImVec2 button_size = ImVec2(-1.0f, 30.0f);

		for (int i = 0; i < demos.size(); i++)
		{
			if (!isLoading && ImGui::Button(demos[i], button_size))
			{
				loadingDemoIndex = i;
				isLoading = true;
			}
		}

		// Draw loading text at bottom center if loading
		if (isLoading)
		{
			loadingCounter++;

			ImGui::Dummy(ImVec2(0, 10)); // spacer

			// Get available content region width & cursor position
			ImVec2 avail = ImGui::GetWindowContentRegionMax();
			float windowWidth = avail.x;

			// Set font scale for big text
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 10); // small vertical padding

			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 0, 0, 1)); // black text
			ImGui::SetCursorPosX((windowWidth - ImGui::CalcTextSize("LOADING...").x) * 0.5f);
			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[0]); // Optional: replace with your big font if you have one
			ImGui::Text("LOADING...");
			ImGui::PopFont();
			ImGui::PopStyleColor();
		}

		if (isLoading && loadingCounter > 4)
		{
			switch (loadingDemoIndex)
			{
				case 0:
					mDirector.PushScene(new StackOfBoxesDemo{mDirector});
					break;
				case 1:
					mDirector.PushScene(new VehiclePhysicsDemo{mDirector});
					break;
				case 2:
					mDirector.PushScene(new SkeletalAnimationDemo{mDirector});
					break;
				case 3:
					mDirector.PushScene(new SkeletalAnimationWithIK{mDirector});
					break;
				case 4:
					mDirector.PushScene(new ShadowsDemo{mDirector});
					break;
				case 5:
					mDirector.PushScene(new FrustumCullingDemo{mDirector});
					break;
			}

			isLoading = false;
			loadingCounter = 0;
			loadingDemoIndex = -1;
		}

		ImGui::EndChild();
		ImGui::End();

		ImGui::PopStyleVar();
		ImGui::PopStyleColor(7);
	}

	void MainEntryScene::OnResume()
	{
		mDirector.HideMouse(false);
		CurrentCamera()->SetEnabled(false);
	}

	void MainEntryScene::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		EkScene::DebugDraw(debugRenderer);
	}
} // namespace Eklavya
