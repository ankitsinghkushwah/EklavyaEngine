//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "VehiclePhysicsDemo.hpp"

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
#include "Scene/SceneHelper.hpp"
#include "Scene/Cameras/FreeLookCamera.h"
#include "CarComponent.hpp"

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya {
	enum MatIndex {
		GOLD,
		RUBBER,
		CRATE_SCIFI,
		CRATE_WOOD,
		PLASTIC
	};

	// std::array<MaterialInfo> mMats;

	void VehiclePhysicsDemo::PreloadTextures() {
		std::vector<std::string> texturesList = {"grid", "crate", "grass", "floor"};

		std::string ext = "png";

		for (auto &name: texturesList) {
			std::string folder = "pbr/" + name + "/";

			AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
			AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
			AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
			AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
			AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
		}
	}

	MaterialInfo VehiclePhysicsDemo::LoadMaterialInfo(const std::string &file, std::string ext) {
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

	std::string tire = "truck/tire2";
	std::string chassis = "truck/chassis";

	void VehiclePhysicsDemo::CreateStage() {
		MaterialInfo info;
		info.mRoughness = 1.0f;
		info.mTiling = 40;
		info.mBaseColor = glm::vec3(0.0f, 0.6f, 0.2f);

		float area_extent = 10000;
		float floorScaleY = 10.0f;
		CreateCube(glm::vec3(0.0f), glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(), FLT_MAX, info, 0);

		MaterialInfo infoGrass = LoadMaterialInfo("crate");
		infoGrass.mTiling = 2;

		float scaleY = 1.0f;
		float pos = floorScaleY / 2 + scaleY / 2;

		for (int z = 0; z < 5; z++) {
			CreateCube(glm::vec3(0.0, pos, 10 + z * 10.0f), glm::vec3(30.0f, 2.0f, scaleY + 2),
			           glm::vec3(0.0f, 0.0f, 0.0f), FLT_MAX, infoGrass, 0);
		}

		//
		CreateCube(glm::vec3(0.0, 17, -100.0f), glm::vec3(100.0f, 7.0f, 120),
		           glm::vec3(glm::radians(15.0f), 0.0f, 0.0f), FLT_MAX, infoGrass, 0);
		//
		CreateCube(glm::vec3(0.0, 15, -300.0f), glm::vec3(100.0f, 7.0f, 120),
		           glm::vec3(glm::radians(-15.0f), 0.0f, 0.0f), FLT_MAX, infoGrass, 0);
	}

	void VehiclePhysicsDemo::CreateCar() {
		MaterialInfo info;
		info.mBaseColor = glm::vec3(0.7f);
		info.mRoughness = 0.5f;
		info.mMetallic = 1.0f;

		glm::vec3 carScale(8, 7, 24);
		UniqueActor car = SceneHelper::CreateActorFromModel(chassis, 0);
		car->Transform().SetScale(glm::vec3(3.0f));

		auto collider = car->EmplaceComponent<Physics::BoxColliderComponent>();
		glm::vec3 colliderScale = carScale;
		colliderScale.z *= 0.8f;
		collider->SetHalfSize(carScale);
		collider->SetGroupIndex(1);
		collider->SetOffset(glm::vec3(0.0f, 2.9f, 0.0f));

		mChassisBody = car->EmplaceComponent<Physics::EkBody>();
		mChassisBody->SetMass(200.0f);
		mChassisBody->SetPosition(glm::vec3(0.0f, 10.0f, 0.0f));
		mChassisBody->SetRotation(glm::vec3(0.0f));
		mChassisBody->SetRotation(glm::vec3(1.0f));
		mChassisBody->SetLinearDamping(0.99);
		mChassisBody->SetAngularDamping(0.99);

		car->EmplaceComponent<CarComponent>();

		// FORWARD-RIGHT SUSPENSION
		for (int i = 0; i < 4; i++) {
			UniqueActor uniqueSuspension = std::make_unique<EkActor>();
			uniqueSuspension->SetName("suspension");
			EkActor *suspension = uniqueSuspension.get();
			car->AddKid(uniqueSuspension);

			UniqueActor uniqueWheel = SceneHelper::CreateActorFromModel(tire, 1);
			uniqueWheel->Transform().SetScale(6.0f);
			EkActor *wheel = uniqueWheel.get();
			AddActor(uniqueWheel);

			float offX = -0.5;
			float offZ = -3.5;
			float xOff = ((carScale.x) * 0.25) + offX;
			float zOff = ((carScale.z) * 0.25) + offZ;
			float yOff = -0.7f;
			if (i == 0)
				suspension->Transform().SetPosition(xOff, yOff, zOff - 0.2f);
			else if (i == 1)
				suspension->Transform().SetPosition(-xOff, yOff, zOff - 0.2f);
			else if (i == 2)
				suspension->Transform().SetPosition(xOff, yOff, -zOff);
			else if (i == 3)
				suspension->Transform().SetPosition(-xOff, yOff, -zOff);

			mSuspensions[i] = suspension->EmplaceComponent<CarSuspension>(this, i);
			mSuspensions[i]->mWheel = wheel;
		}

		car->Transform().SetPosition(0, 100, 0);

		std::shared_ptr<SpringFollowCamera> followCamera = std::make_shared<SpringFollowCamera>(mDefaultCameraParams);

		followCamera->SetTarget(&car->Transform());
		followCamera->SetArmsLength(glm::vec3(0.0f, 20.0f, 70.0f));
		followCamera->SetTargetOffset(glm::vec3(0.0f, 0.0f, -400.0f));

		this->OverrideCamera(followCamera);

		AddActor(car);
	}

	void VehiclePhysicsDemo::LoadMesh() {
		std::string name = "kart/kart";

		UniqueActor actor = SceneHelper::CreateActorFromModel(name, 0);

		AddActor(actor);
	}

	VehiclePhysicsDemo::VehiclePhysicsDemo(Director *pDirector) : MainEntryScene(pDirector) {
		// Load assets

		PreloadTextures();

		AssetManager::GetInstance().LoadModel(chassis, ".gltf", 0);
		AssetManager::GetInstance().LoadModel(tire, ".gltf", 1);

		CreateStage();
		CreateCar();
	}

	VehiclePhysicsDemo::~VehiclePhysicsDemo() {
	}

	void VehiclePhysicsDemo::OnKeyAction(int key, int action) {
		MainEntryScene::OnKeyAction(key, action);
		if (key == GLFW_KEY_H && action == GLFW_PRESS)
			CreateCubeStack();
	}

	void VehiclePhysicsDemo::CreateCubeStack() {
		int rows = 5;
		int cols = 6;
		float startY = 50.0f;
		float boxDim = 10.0f;
		float offsetX = ((cols - 1) / 2.0f) * boxDim;

		for (int r = 0; r < rows; ++r) {
			for (int c = 0; c < cols; ++c) {
				float x = (c * boxDim) - offsetX;
				float y = startY + r * boxDim;
				float z = 0.0f;

				MaterialInfo info = LoadMaterialInfo("crate");
				info.mRoughness = 1.0f;
				info.mMetallic = 0.0f;
				info.mTiling = 1;
				info.mBaseColor = Random::GetInstance()->GetPointOnUnitSphere();
				CreateCube(glm::vec3(x, y, z), glm::vec3(boxDim), glm::vec3(glm::radians(0.0f), 0.0f, 0.0f), 5.0f, info,
				           0);

				//CreateSphere(glm::vec3(x, y, z), boxDim, 20.0f, info);
			}
		}
	}

#ifdef EKDEBUG

	void VehiclePhysicsDemo::ImGuiProc() {
		MainEntryScene::ImGuiProc();
	}

	void VehiclePhysicsDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer) {
		MainEntryScene::DebugDraw(debugRenderer);
		for (int i = 0; i < 4; i++) {
			mSuspensions[i]->DebugDraw(debugRenderer);
		}
	}
#endif
} // namespace Eklavya
