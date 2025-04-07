//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "imgui/imgui.h"
#include <AssetManager/AssetManager.h>
#include <Components/RenderComponent.hpp>
#include <Director.hpp>
#include <EkPhysics/Collider.h>
#include <EkPhysics/EkBody.h>
#include <MainEntryScene.hpp>
#include <Random.h>
#include <Renderer/GLRenderer.h>
#include <Renderer/Material.h>
#include <Scene/EkActor.h>
#include <Scene/SceneHelper.hpp>
#include <Scene/SpringFollowCamera.hpp>

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

	UniqueActor MainEntryScene::CreateCube(glm::vec3 pos, glm::vec3 scale, glm::vec3 rotate, float mass,
		Asset::MaterialInfo matInfo, uint32_t flag, bool kid)
	{
		UniqueActor floor = std::make_unique<EkActor>();
		floor->Transform().SetScale(scale);
		floor->Transform().SetPosition(pos);
		RenderComponent* render = floor->EmplaceComponent<RenderComponent>(EMeshType::CUBE);
		render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
		render->GetMesh().mMaterialInfo = matInfo;

		if (!kid)
		{
			auto collider = floor->EmplaceComponent<Physics::BoxColliderComponent>();

			collider->SetHalfSize(floor->Transform().GetScale());
			collider->SetGroupIndex(flag);

			auto body = floor->EmplaceComponent<Physics::EkBody>();
			body->SetMass(mass);
			body->SetPosition(pos);
			body->SetRotation(rotate);

			AddActor(floor);
		}

		return floor;
	}

	UniqueActor MainEntryScene::CreateSphere(glm::vec3 pos, float radius, float mass, Asset::MaterialInfo matInfo,
		uint32_t flag, bool kid)
	{
		UniqueActor sphere = std::make_unique<EkActor>();
		sphere->Transform().SetScale(radius);
		sphere->Transform().SetPosition(pos);
		auto render = sphere->EmplaceComponent<RenderComponent>(EMeshType::SPHERE);
		render->mRenderGroup = Renderer::ERenderGroup::ACTOR;
		render->GetMesh().mMaterialInfo = matInfo;


		if (!kid)
		{
			auto collider = sphere->EmplaceComponent<Physics::SphereColliderComponent>();
			collider->SetRadius(radius);
			collider->SetGroupIndex(flag);

			auto body = sphere->EmplaceComponent<Physics::EkBody>();
			body->SetMass(mass);
			body->SetPosition(pos);
			body->SetRestitution(.4f);

			AddActor(sphere);
		}

		return sphere;
	}

	MainEntryScene::MainEntryScene(Director& pDirector) :
		EkScene(pDirector)
	{
		GetRenderer().GetMaterialForGroup<SkyboxMaterial>(Renderer::ERenderGroup::SKYBOX)->mCubemap =
			AssetManager::GetInstance().LoadCubemap("Day", "png");

		UniqueActor sky = std::make_unique<EkActor>();
		RenderComponent* render = sky->EmplaceComponent<RenderComponent>(EMeshType::CUBE);
		render->mProjectsShadow = false;
		render->mRenderGroup = Renderer::ERenderGroup::SKYBOX;
		AddActor(sky);
	}

	MainEntryScene::~MainEntryScene() {}

	void MainEntryScene::ImGuiProc()
	{
		EkScene::ImGuiProc();
	}

#ifdef EKDEBUG
	void MainEntryScene::DebugDraw(Renderer::DebugRenderer& debugRenderer)
	{
		EkScene::DebugDraw(debugRenderer);
	}


#endif
} // namespace Eklavya
