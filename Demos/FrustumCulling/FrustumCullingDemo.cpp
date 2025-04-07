//
// Created by Ankit Singh Kushwah on 16/03/2025.
//

#include "AssetManager/AssetManager.h"
#include "Components/RenderComponent.hpp"
#include "EkPhysics/Collider.h"
#include "FrustumCullingDemo.h"
#include "Random.h"
#include "imgui/imgui.h"


using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	FrustumCullingDemo::FrustumCullingDemo(Director &pDirector) :
		MainEntryScene(pDirector)
	{
		PreloadTextures();
		CreateStage();
	}

	FrustumCullingDemo::~FrustumCullingDemo() {}

	void FrustumCullingDemo::OnMouseAction(int key, int action)
	{
		MainEntryScene::OnMouseAction(key, action);
	}

	void FrustumCullingDemo::PreloadTextures()
	{
		std::string ext = "png";

		std::vector<std::string> texturesList = {"grid", "crate", "grass", "floor"};

		for (auto &name: texturesList)
		{
			std::string folder = "pbr/" + name + "/";

			AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
			AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
			AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
			AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
			AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
		}
	}

	MaterialInfo FrustumCullingDemo::LoadMaterialInfo(const std::string &file, std::string ext)
	{
		MaterialInfo info;

		std::string folder = "pbr/" + file + "/";
		info.mBaseMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "albedo");
		info.mNormalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "normal");
		info.mAOMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "ao");
		info.mRoughMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "roughness");
		info.mMetalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "metallic");

		return info;
	}

	void FrustumCullingDemo::CreateStage()
	{
		MaterialInfo info = LoadMaterialInfo("grid");
		info.mRoughness = 1.0f;
		info.mTiling = 40;
		info.mBaseColor = glm::vec3(0.0f, 0.6f, 0.2f);

		float area_extent = 10000;
		float floorScaleY = 10.0f;
		CreateCube(glm::vec3(0.0f, -100.0f, 0.0f), glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(),
		           FLT_MAX, info, 0);

		auto xzRotationParent = std::make_unique<EkActor>();


		const float offset = 10.0f;
		for (float angle = 0; angle <= 360.0f; angle += offset)
		{
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			pos.x = cos(glm::radians(angle)) * 100.0f;
			pos.z = sin(glm::radians(angle)) * 100.0f;
			glm::vec3 scale = glm::vec3(10.0f);
			glm::vec3 rotation = glm::vec3(0.0f, angle, 0.0f);
			MaterialInfo info = LoadMaterialInfo("crate");
			info.mRoughness = 1.0f;
			info.mMetallic = 0.0f;
			info.mTiling = 1;
			info.mBaseColor = Random::GetInstance()->GetPointOnUnitSphere();
			UniqueActor actor = CreateCube(pos, scale, rotation, 30.0f, info, 0, true);
			xzRotationParent->AddKid(actor);
		}

		mXZRotationParent = xzRotationParent.get();
		AddActor(xzRotationParent);

		auto yzRotationParent = std::make_unique<EkActor>();


		for (float angle = 0; angle <= 360.0f; angle += offset)
		{
			glm::vec3 pos = glm::vec3(0.0f, 0.0f, 0.0f);
			pos.z = cos(glm::radians(angle)) * 70.0f;
			pos.y = sin(glm::radians(angle)) * 70.0f;
			glm::vec3 scale = glm::vec3(10.0f);
			glm::vec3 rotation = glm::vec3(0.0f, angle, 0.0f);
			MaterialInfo info = LoadMaterialInfo("grass");
			info.mRoughness = 1.0f;
			info.mMetallic = 0.0f;
			info.mTiling = 1;
			info.mBaseColor = Random::GetInstance()->GetPointOnUnitSphere();
			UniqueActor actor = CreateCube(pos, scale, rotation, 30.0f, info, 0, true);
			yzRotationParent->AddKid(actor);
		}

		mYXRotationParent = yzRotationParent.get();
		AddActor(yzRotationParent);
	}

	void FrustumCullingDemo::OnKeyAction(int key, int action)
	{
		MainEntryScene::OnKeyAction(key, action);
	}

	void FrustumCullingDemo::Tick(float deltaTime)
	{
		MainEntryScene::Tick(deltaTime);

		static float angle = 0.0f;
		angle += deltaTime;
		mXZRotationParent->Transform().SetRotation(glm::vec3(0.0f, angle, 0.0f));

		static float angle2 = 0.0f;
		angle2 += deltaTime;
		mYXRotationParent->Transform().SetRotation(glm::vec3(angle2, 0.0f, 0.0f));
	}

	void FrustumCullingDemo::ImGuiProc()
	{
		MainEntryScene::ImGuiProc();

		if (ImGui::Begin("Frustum Culling Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Press G to freeze the frustum!");
			ImGui::End();
		}
	}

	void FrustumCullingDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);
	}
} // namespace Eklavya
