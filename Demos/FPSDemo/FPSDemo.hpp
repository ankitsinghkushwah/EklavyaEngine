//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_FPS_DEMO_H_
#define INC_FPS_DEMO_H_

#include <MainEntryScene.hpp>
#include <UserInputListener.h>
#include "Material.h"
#include "MaterialInfo.h"
#include <SFML/Audio.hpp>
#include "SceneHelper.hpp"

namespace Eklavya
{
  class FPSDemo final : public MainEntryScene
  {
  public:
    FPSDemo(class Director *pDirector);

    ~FPSDemo() override;

  private:
    void                PreloadTextures();

    Asset::MaterialInfo LoadMaterialInfo(const std::string &file,
                                         std::string        ext = "png");
    void                ImGuiProc() override;

    void                CreateStage();

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
    std::vector<std::string>                                       mCharacters;
    std::unordered_map<std::string, std::vector<SHARED_ANIMATION>> mAnimations;
  };

} // namespace Eklavya

#endif
