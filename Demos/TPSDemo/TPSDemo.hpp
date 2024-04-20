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
#include "Material.h"
#include "MaterialInfo.h"
#include "CarSuspension.hpp"
#include <SFML/Audio.hpp>
#include "SceneHelper.hpp"
#include "PlayerController.hpp"
#include "Scene/Cameras/SpringFolllowCamera.h"

namespace Eklavya
{
  class TPSDemo final : public MainEntryScene
  {
  public:
    TPSDemo(class Director *pDirector);

    ~TPSDemo() override;

  private:
    void                PreloadTextures();

    Asset::MaterialInfo LoadMaterialInfo(const std::string &file,
                                         std::string        ext = "png");
    void                ImGuiProc() override;

    void LoadLevel(std::string name);
    void                CreateStage();

    void                SetupPlayer();

    void                LoadCharactersAndAnimations();
    void                InstantiateCharacters();
    SceneHelper::ModelLoadOptions
                GetMaterialsForCharacter(const std::string &character);
    std::string GetExtension(const std::string &character);
    std::vector<std::string> AnimationsToLoad(const std::string &animations);

    std::string              ModelName(const std::string &character)
    {
      return "characters/" + character + "/" + character;
    }

    std::string AnimationName(const std::string &character,
                              const std::string &animation)
    {
      return "characters/" + character + "/" + animation;
    }

#ifdef EKDEBUG
    void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;
#endif

  private:
    SHARED_ACTOR                                 mPlayer = nullptr;
    SHARED_PLAYER_CONTROLLER                     mPlayerController = nullptr;

    std::shared_ptr<Eklavya::SpringFollowCamera> mFollowCamera = nullptr;

    std::vector<std::string>                     mCharacters;
    std::unordered_map<std::string, std::vector<SHARED_ANIMATION>> mAnimations;
  };

} // namespace Eklavya

#endif
