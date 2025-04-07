//
// Created by Ankit Singh Kushwah on 16/03/2025.
//
#include "ShadowsDemo.h"
#include "Components/RenderComponent.hpp"
#include "AssetManager/AssetManager.h"
#include "Random.h"
#include "EkPhysics/Collider.h"

using namespace Eklavya;
using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	ShadowsDemo::ShadowsDemo(Director &pDirector) :
		MainEntryScene(pDirector)
	{
		PreloadTextures();
		CreateStage();
	}

	ShadowsDemo::~ShadowsDemo() {}

	void ShadowsDemo::OnMouseAction(int key, int action)
	{
		MainEntryScene::OnMouseAction(key, action);
	}

	void ShadowsDemo::PreloadTextures()
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

	MaterialInfo ShadowsDemo::LoadMaterialInfo(const std::string &file, std::string ext)
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


	void ShadowsDemo::CreateStage()
	{
		MaterialInfo info = LoadMaterialInfo("grid");
		info.mBaseColor = glm::vec4(0.6, 0.6f, 0.6f, 0.6f);
		info.mRoughness = .5f;
		info.mTiling = 10;
		info.mBaseColor = glm::vec3(0.0f, 0.6f, 0.2f);

		float area_extent = 2000;
		float floorScaleY = 10.0f;

		AddBox(glm::vec3(0.0f), glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(), FLT_MAX, info, 0);

		const char *base = "tree/scene";
		AssetManager::GetInstance().LoadModel(base, ".gltf", 0);

		std::unique_ptr<EkActor> base_actor = SceneHelper::CreateActorFromModel(base, 0);

		base_actor->Transform().SetPosition(0.0f, 260.0f, 0.0f);
		base_actor->Transform().SetRotation(glm::vec3(0.0f, 90.0f, 0.0f));
		base_actor->Transform().SetScale(0.35f);
		AddActor(base_actor);
	}

	void ShadowsDemo::OnKeyAction(int key, int action)
	{
		MainEntryScene::OnKeyAction(key, action);
	}


#ifdef EKDEBUG
	void ShadowsDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);
	}

	void ShadowsDemo::ImGuiProc()
	{
		MainEntryScene::ImGuiProc();
	}
#endif
}
