//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "SkeletalAnimationWithIK.hpp"

#include <InputHandler.h>
#include "Components/RenderComponent.hpp"
#include <Scene/EkActor.h>
#include "AssetManager/AssetManager.h"
#include "Director.hpp"
#include <Renderer/Material.h>
#include "imgui/imgui.h"
#include "Random.h"
#include "EkPhysics/EkBody.h"
#include "EkPhysics/Collider.h"
#include <memory>
#include "Scene/Cameras/FreeLookCamera.h"

#include "AssetManager/Animation.h"
#include "PlayerController.hpp"
#include "Components/AnimationIKSolverComponent.hpp"

#include "Scene/Cameras/SpringFolllowCamera.h"

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	void SkeletalAnimationWithIK::PreloadTextures()
	{
		std::string ext = "png";

		for (auto &name: mCharacters)
		{
			std::string folder = "characters/" + name + "/";

			AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
			AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
			AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
			AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
			AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
		}

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

	MaterialInfo SkeletalAnimationWithIK::LoadMaterialInfo(const std::string &file, std::string ext)
	{
		MaterialInfo info;

		std::string folder = file + "/";
		info.mBaseMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "albedo");
		info.mNormalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "normal");
		info.mAOMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "ao");
		info.mRoughMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "roughness");
		info.mMetalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "metallic");

		return info;
	}

	std::string SkeletalAnimationWithIK::GetExtension(const std::string &character)
	{
		if (character == "player")
		{
			return ".fbx";
		}
		else if (character == "swat")
		{
			return ".dae";
		}

		return ".fbx";
	}

	void SkeletalAnimationWithIK::LoadCharactersAndAnimations()
	{
		int modelID = 0;
		for (std::string &character: mCharacters)
		{
			std::string ext = GetExtension(character);
			AssetManager::GetInstance().LoadModel(ModelName(character), ext, modelID);
			std::vector<std::string> animations = AnimationsToLoad(character);
			for (auto &animation: animations)
			{
				AssetManager::GetInstance().LoadAnimation(AnimationName(character, animation), ext, modelID);
			}
			modelID++;
		}
	}

	SceneHelper::ModelLoadOptions SkeletalAnimationWithIK::GetMaterialsForCharacter(const std::string &character)
	{
		SceneHelper::ModelLoadOptions loadOptions;
		SceneHelper::MATERIALS_FOR_ACTOR materials;
		if (character == "player")
		{
			loadOptions.forceSingleMaterial = true;
			Asset::MaterialInfo materialInfo = LoadMaterialInfo("characters/" + character);
			materialInfo.mMeshName = "";
			materials.push_back(materialInfo);
		}
		loadOptions.materials = std::move(materials);
		return loadOptions;
	}

	std::vector<std::string> SkeletalAnimationWithIK::AnimationsToLoad(const std::string &character)
	{
		std::vector<std::string> animationsToLoad;

		if (character == "player")
		{
			// movement
			animationsToLoad.push_back("standing_movement/idle");
			animationsToLoad.push_back("standing_movement/walk_forward");
			animationsToLoad.push_back("standing_movement/walk_back");
			animationsToLoad.push_back("standing_movement/walk_left");
			animationsToLoad.push_back("standing_movement/walk_right");
			animationsToLoad.push_back("standing_movement/run_forward");
			animationsToLoad.push_back("standing_movement/run_back");
			animationsToLoad.push_back("standing_movement/run_left");
			animationsToLoad.push_back("standing_movement/run_right");

			// jumping

			animationsToLoad.push_back("jumping/takeoff");
			animationsToLoad.push_back("jumping/flight");
			animationsToLoad.push_back("jumping/land");
		}
		if (character == "swat")
		{
			animationsToLoad.push_back("animations/crouch_idle");
			animationsToLoad.push_back("animations/crouch_turn_left");
			animationsToLoad.push_back("animations/crouch_turn_right");
			animationsToLoad.push_back("animations/crouch_jog_forward");
			animationsToLoad.push_back("animations/standing_idle");
			animationsToLoad.push_back("animations/standing_turn_left");
			animationsToLoad.push_back("animations/standing_turn_right");
			animationsToLoad.push_back("animations/standing_jog_forward");
			animationsToLoad.push_back("animations/standing_jog_backward");
		}

		if (character == "new_soldier")
		{
			animationsToLoad.push_back("idle");
		}

		return animationsToLoad;
	}


	void SkeletalAnimationWithIK::CreateStage()
	{
		MaterialInfo info = LoadMaterialInfo("pbr/grid");
		info.mRoughness = 1.0f;
		info.mTiling = 40;

		float areaAxtent = 10000;
		float floorScaleY = 30.0f;
		AddBox(glm::vec3(0.0f), glm::vec3(areaAxtent, floorScaleY, areaAxtent), glm::vec3(), FLT_MAX, info, 0);

		ikFloor = AddBox(glm::vec3(0.0f, 0.0f, -700.0f), glm::vec3(500.0f, 30.0f, 1000.0f),
		                 glm::vec3(glm::radians(floorAngle), 0.0f, 0.0f), FLT_MAX, info, 0);

		ikFloor->mDebugDrawComponents = true;
	}

	void SkeletalAnimationWithIK::SetupPlayer()
	{
		std::string character = "swat";
		int modelID = 0;

		SceneHelper::ModelLoadOptions materials = GetMaterialsForCharacter(character);
		std::vector<std::string> animationsToLoad = AnimationsToLoad(character);

		MaterialInfo info;
		info.mBaseColor = glm::vec3(0.7f);
		info.mRoughness = 0.5f;
		info.mMetallic = 1.0f;

		glm::vec3 scale(.5f);

		UniqueActor playerActor = SceneHelper::CreateActorFromModel(ModelName(character), modelID, materials);
		playerActor->mDebugDrawComponents = true;
		//playerActor->EmplaceComponent<AnimationComponent>(modelID);
		playerActor->EmplaceComponent<AnimationIKSolver>(*this);

		playerActor->Transform().SetScale(scale);

		playerActor->SetName("TPS Character");

		playerActor->Transform().SetPosition(0.0f, 30.0f, 0.0f);

		// mPlayerController = playerActor->EmplaceComponent<PlayerController>(*this);
		// mPlayerController->Init();

		this->playerActor = playerActor.get();
		AddActor(playerActor);
	}

	SkeletalAnimationWithIK::SkeletalAnimationWithIK(Director &pDirector) :
		MainEntryScene(pDirector)
	{
		mDirector.HideMouse();
		CurrentCamera()->SetEnabled(true);

		mCharacters.push_back("swat");
		PreloadTextures();
		LoadCharactersAndAnimations();
		CreateStage();
		SetupPlayer();
	}

	void SkeletalAnimationWithIK::Tick(float dt)
	{
		MainEntryScene::Tick(dt);


		float s = 10.0f;

		glm::vec3 pos = this->playerActor->Transform().Position();
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_U))
		{
			pos.y -= s * dt;
		}
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_I))
		{
			pos.y += s * dt;
		}

		this->playerActor->Transform().SetPosition(pos);
	}

	SkeletalAnimationWithIK::~SkeletalAnimationWithIK() {}


	void SkeletalAnimationWithIK::ImGuiProc()
	{
		EkScene::ImGuiProc();

		if (ImGui::Begin("Inverse Kinematics Demo", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Press H & J to move left leg target position UP & Down");
			ImGui::Text("Press K & L to move left leg target forward and backward");
			ImGui::Text("Press U & I to move the character UP & Down");

			ImGui::Text("PRESS ESCAPE TO GO BACK TO DEMO OPTIONS");
			ImGui::End();
		}
	}

	void SkeletalAnimationWithIK::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);
		//mPlayerController->DebugDraw(debugRenderer);
	}
} // namespace Eklavya
