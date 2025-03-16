//
//  IDemoScene.h
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//
#ifndef INC_VEHICLE_PHYSICS_DEMO_H_
#define INC_VEHICLE_PHYSICS_DEMO_H_

#include <MainEntryScene.hpp>
#include <UserInputListener.h>
#include <Renderer/Material.h>
#include <AssetManager/MaterialInfo.h>
#include "CarSuspension.hpp"
#include <SFML/Audio.hpp>
#include "Scene/Cameras/SpringFolllowCamera.h"

namespace Eklavya
{
	class VehiclePhysicsDemo final : public MainEntryScene
	{
	public:
		VehiclePhysicsDemo(class Director &pDirector);

		~VehiclePhysicsDemo() override;

		void OnKeyAction(int key, int action) override;

	private:
		void CreateCubeStack();

		std::array<CarSuspension *, 4> mSuspensions;

		Physics::EkBody *mChassisBody = nullptr;

		void PreloadTextures();

		Asset::MaterialInfo LoadMaterialInfo(const std::string &file, std::string ext = "png");

		void CreateStage();

		void CreateCar();

		void LoadMesh();

#ifdef EKDEBUG
		void ImGuiProc() override;

		void DebugDraw(Renderer::DebugRenderer &debugRenderer) override;
#endif
	};
} // namespace Eklavya

#endif
