//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_TPS_DEMO_H_
#define INC_TPS_DEMO_H_

#include <MainEntryScene.hpp>
#include <UserInputListener.h>
#include <Renderer/Material.h>
#include <AssetManager/MaterialInfo.h>
#include <SFML/Audio.hpp>
#include "Scene/SceneHelper.hpp"
#include "PlayerController.hpp"
#include "Scene/Cameras/SpringFolllowCamera.h"

namespace Eklavya
{
	class SkeletalAnimationWithIK final : public MainEntryScene
	{
	public:
		SkeletalAnimationWithIK(class Director& pDirector);

		~SkeletalAnimationWithIK() override;

	private:
		void PreloadTextures();

		Asset::MaterialInfo LoadMaterialInfo(const std::string& file, std::string ext = "png");

		void LoadLevel(std::string name);

		void CreateStage();

		void SetupPlayer();

		void Tick(float dt) override;

		void LoadCharactersAndAnimations();

		void InstantiateCharacters();

		SceneHelper::ModelLoadOptions GetMaterialsForCharacter(const std::string& character);

		std::string GetExtension(const std::string& character);

		std::vector<std::string> AnimationsToLoad(const std::string& animations);

		std::string ModelName(const std::string& character)
		{
			return "characters/" + character + "/" + character;
		}

		std::string AnimationName(const std::string& character, const std::string& animation)
		{
			return "characters/" + character + "/" + animation;
		}

#ifdef EKDEBUG
		void ImGuiProc() override;

		void DebugDraw(Renderer::DebugRenderer& debugRenderer) override;
#endif

	private:
		EkActor* mPlayer = nullptr;
		PlayerController* mPlayerController = nullptr;

		std::shared_ptr<Eklavya::SpringFollowCamera> mFollowCamera = nullptr;

		std::vector<std::string> mCharacters;
		std::unordered_map<std::string, std::vector<SHARED_ANIMATION> > mAnimations;

		float floorAngle = 30.0f;
		UniqueActor ikFloor = nullptr;

		glm::vec3 ikTarget;
	};
} // namespace Eklavya

#endif
