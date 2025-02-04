//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "SkeletalAnimationDemo.hpp"

#include <InputHandler.h>
#include "RenderComponent.hpp"
#include "EkActor.h"
#include "AssetManager.h"
#include "Director.hpp"
#include "Material.h"
#include "imgui.h"
#include "Random.h"
#include "EkPhysics/EkBody.h"
#include "EkPhysics/Collider.h"
#include <memory>
#include "SceneHelper.hpp"
#include "FreeLookCamera.h"
#include "CarComponent.hpp"
#include "Animation.h"

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

	// std::array<MaterialInfo> mMats;

	void SkeletalAnimationDemo::PreloadTextures()
	{
		std::vector<std::string> texturesList = {"grid", "crate", "grass", "floor"};

		std::string ext = "png";

		for (auto& name : texturesList)
		{
			std::string folder = "pbr/" + name + "/";

			AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
			AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
			AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
			AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
			AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
		}
	}

	MaterialInfo SkeletalAnimationDemo::LoadMaterialInfo(const std::string& file, std::string ext)
	{
		MaterialInfo info;

		std::string folder = "pbr/" + file + "/";
		info.mBaseMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "albedo");
		info.mNormalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "normal");
		info.mAOMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "ao");
		info.mRoughMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "roughness");
		info.mMetalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "metallic");

		info.mCubemap = GetRenderer().GetMaterialForGroup<SkyboxMaterial>(Renderer::ERenderGroup::SKYBOX)->mCubemap;

		return info;
	}

	void SkeletalAnimationDemo::CreateStage()
	{
		MaterialInfo info = LoadMaterialInfo("grid");
		info.mRoughness = 1.0f;
		info.mTiling = 40;

		float area_extent = 10000;
		float floorScaleY = 5.0f;
		CreateCube(glm::vec3(0.0f), glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(), FLT_MAX, info, STATIC);
	}

	void SkeletalAnimationDemo::LoadMesh()
	{
		std::string name = "characters/swat/swat";

		UniqueActor         actor = SceneHelper::CreateActorFromModel(name, 0);
		AnimationComponent* animator = actor->EmplaceComponent<AnimationComponent>(0);
		SHARED_ANIMATION    animation = AssetManager::GetInstance().GetAsset<Asset::Animation>("characters/swat/animations/standing_jog_forward");

		animator->PlayAnimation(animation);
		actor->Transform().SetScale(1.0f);

		AddActor(actor);
	}

	SkeletalAnimationDemo::SkeletalAnimationDemo(Director* pDirector) : MainEntryScene(pDirector)
	{
		// Load assets

		PreloadTextures();
		AssetManager::GetInstance().LoadModel("characters/swat/swat", ".dae", 0);
		AssetManager::GetInstance().LoadAnimation("characters/swat/animations/standing_jog_forward", ".dae", 0);

		//CreateStage();

		LoadMesh();
	}

	SkeletalAnimationDemo::~SkeletalAnimationDemo()
	{
	}

	void SkeletalAnimationDemo::CreateCubeStack()
	{
		MaterialInfo info;
		info.mBaseColor = glm::vec3(0.7f, 1.0f, 0.4f);
		info.mRoughness = 1.0f;
		info.mMetallic = 1.0f;

		float radius = 5.0f;
		int   rows = 4;
		int   cols = 4;

		float diameter = radius * 2;
		float xOffset = (cols * diameter / 2.0f);
		float yOffset = 5.0f;
		float offset = 10.0f;
		for (int row = 1; row <= rows; ++row)
		{
			for (int col = 1; col <= cols; ++col)
			{
				float x = xOffset + (col * radius);
				float y = yOffset + (row * radius + offset);
				CreateSphere(glm::vec3(x, y, -400.0f), 30.0f, 3.0f, info);
			}
		}
	}


#ifdef EKDEBUG

	void SkeletalAnimationDemo::ImGuiProc()
	{
		MainEntryScene::ImGuiProc();
	}
	void SkeletalAnimationDemo::DebugDraw(Renderer::DebugRenderer& debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);
	}
#endif

} // namespace Eklavya
