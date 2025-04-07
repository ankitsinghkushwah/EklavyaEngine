//
// Created by Ankit Singh Kushwah on 16/03/2025.
//

#include "SceneTemplate.h"
#include "Components/RenderComponent.hpp"
#include "AssetManager/AssetManager.h"
#include "Random.h"
#include "EkPhysics/Collider.h"


using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	SceneTemplate::SceneTemplate(Director &pDirector) :
		MainEntryScene(pDirector)
	{
		PreloadTextures();
		CreateStage();
	}

	SceneTemplate::~SceneTemplate() {}

	void SceneTemplate::OnMouseAction(int key, int action)
	{
		MainEntryScene::OnMouseAction(key, action);
	}

	void SceneTemplate::PreloadTextures()
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

	MaterialInfo SceneTemplate::LoadMaterialInfo(const std::string &file, std::string ext)
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

	void SceneTemplate::CreateStage()
	{
		MaterialInfo info;
		info.mBaseColor = glm::vec4(0.5, 0.0f, 0.0f, 0.6f);
		info.mRoughness = .5f;
		info.mTiling = 10;
		info.mBaseColor = glm::vec3(0.0f, 0.6f, 0.2f);

		float area_extent = 2000;
		float floorScaleY = 10.0f;

		AddBox(glm::vec3(0.0f), glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(), FLT_MAX, info, 0);
	}

	void SceneTemplate::OnKeyAction(int key, int action)
	{
		MainEntryScene::OnKeyAction(key, action);
	}


#ifdef EKDEBUG
	void SceneTemplate::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);
	}

	void SceneTemplate::ImGuiProc()
	{
		MainEntryScene::ImGuiProc();
	}
#endif
} // namespace Eklavya
