//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_STACK_OF_BOXES_DEMO
#define INC_STACK_OF_BOXES_DEMO

#include <MainEntryScene.hpp>
#include <UserInputListener.h>
#include <Renderer/Material.h>
#include <AssetManager/MaterialInfo.h>
#include <SFML/Audio.hpp>
#include "Scene/SceneHelper.hpp"
#include "EkAudio.hpp"

namespace Eklavya
{
	class StackOfBoxesDemo final : public MainEntryScene
	{
	public:
		StackOfBoxesDemo(class Director &pDirector);

		~StackOfBoxesDemo() override;

	private:
		void PreloadTextures();

		Asset::MaterialInfo LoadMaterialInfo(const std::string &file, std::string ext = "png");

		void CreateStackOfBoxes();

		void CreateStage();

		void OnMouseAction(int key, int action) override;

		void OnKeyAction(int key, int action) override;

		void Tick(float deltaTime) override;

		void ImGuiProc() override;
#ifdef EKDEBUG
		void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;
#endif

	private:
		Physics::CastHitResult mLastCastHitResult;
		Eklavya::Audio mAudio;
		std::string mEngineLoopSound;
		Physics::EkBody *mFloor = nullptr;
	};
} // namespace Eklavya

#endif
