//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_SKELETAL_ANIMATION_DEMO_H_
#define INC_SKELETAL_ANIMATION_DEMO_H_

#include <MainEntryScene.hpp>
#include <UserInputListener.h>
#include "Material.h"
#include "MaterialInfo.h"
#include "CarSuspension.hpp"
#include <SFML/Audio.hpp>

namespace Eklavya
{
	class SkeletalAnimationDemo final : public MainEntryScene
	{
	  public:
		SkeletalAnimationDemo(class Director* pDirector);

		~SkeletalAnimationDemo() override;

	  private:
		void CreateCubeStack();

		void PreloadTextures();

		Asset::MaterialInfo LoadMaterialInfo(const std::string& file, std::string ext = "png");
		void                ImGuiProc() override;

		void CreateStage();

		void LoadMesh();

#ifdef EKDEBUG
		void DebugDraw(Renderer::DebugRenderer& debugRenderer) override;
#endif
	};

} // namespace Eklavya

#endif
