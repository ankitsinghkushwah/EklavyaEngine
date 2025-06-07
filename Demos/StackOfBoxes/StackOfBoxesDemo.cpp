//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "StackOfBoxesDemo.hpp"
#include <InputHandler.h>
#include "Components/RenderComponent.hpp"
#include <Scene/EkActor.h>
#include "AssetManager/AssetManager.h"
#include <Renderer/Material.h>
#include "imgui/imgui.h"
#include "Random.h"
#include "EkPhysics/EkBody.h"
#include "EkPhysics/Collider.h"
#include <memory>
#include "Scene/Cameras/FreeLookCamera.h"
#include "Director.hpp"

#include "glfw/glfw3.h"

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	StackOfBoxesDemo::StackOfBoxesDemo(Director &pDirector) :
		MainEntryScene(pDirector), mEngineLoopSound("StackOfBoxesDemo/pop.mp3")
	{
		mDirector.HideMouse();
		CurrentCamera()->SetEnabled(true);

		PreloadTextures();

		CreateStage();
		mAudio.Load({mEngineLoopSound});
		mAudio.GetSound().setRelativeToListener(false);
		mAudio.GetSound().setVolume(100.0f);
		mAudio.GetSound().setMinDistance(400.0f);
		mAudio.GetSound().setAttenuation(5.0f);
		mAudio.GetSound().setLoop(false);
	}

	StackOfBoxesDemo::~StackOfBoxesDemo() {}

	void StackOfBoxesDemo::PreloadTextures()
	{
		std::string ext = "png";

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

	MaterialInfo StackOfBoxesDemo::LoadMaterialInfo(const std::string &file, std::string ext)
	{
		MaterialInfo info;

		std::string folder = "pbr/" + file + "/";
		info.mBaseMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "albedo");
		info.mNormalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "normal");
		info.mAOMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "ao");
		info.mRoughMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "roughness");
		info.mMetalMap = AssetManager::GetInstance().GetAsset<Texture2D>(folder + "metallic");

		return info;
	}

	void StackOfBoxesDemo::CreateStage()
	{
		MaterialInfo info = LoadMaterialInfo("grid");

		auto actor = AddBox(glm::vec3(0.0f),
		                    glm::vec3(1000.0f, 30.0f, 1000.0f), glm::vec3(0.0f), FLT_MAX, info, STATIC);

		mFloor = actor->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
	}

	void StackOfBoxesDemo::OnKeyAction(int key, int action)
	{
		MainEntryScene::OnKeyAction(key, action);
		if (key == GLFW_KEY_H && action == GLFW_PRESS)
			CreateStackOfBoxes();
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			int objectsToSkip = 0; //skybox & floor
			for (UniqueActor &actor: mRootActors)
			{
				objectsToSkip++;
				if (objectsToSkip < 2) continue;
				RemoveActor(actor->ID());
			}
			CreateStage();
		}
	}

	void StackOfBoxesDemo::OnMouseAction(int key, int action)
	{
		if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
		{
			glm::vec3 dir = glm::normalize(CurrentCamera()->Forward());
			glm::vec3 pos = CurrentCamera()->Position() + dir * 10.0f;

			MaterialInfo info;
			info.mRoughness = 0.0f;
			info.mMetallic = 0.0f;

			info.mBaseColor = glm::vec4((Random::GetInstance()->GetPointOnUnitSphere() * .1f) + glm::vec3(.1), 1.0f);
			EkActor *sphere = AddSphere(pos, Random::GetInstance()->Real(0.4, 2.0f) * 15.0f, 60.0f, info, -1);
			EkBody *sphereBody = sphere->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
			sphereBody->SetVelocity(CurrentCamera()->Forward() * 300.0f);

			mAudio.Play(mEngineLoopSound, false);
		}
	}


	void StackOfBoxesDemo::Tick(float deltaTime)
	{
		MainEntryScene::Tick(deltaTime);
		float rotSpeed = glm::radians(5.0f);
		glm::vec3 eulers = glm::eulerAngles(mFloor->GetOwner().Transform().Rotation());
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_LEFT))
		{
			eulers.x += rotSpeed * deltaTime;
			mFloor->SetRotation(eulers);
		}
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_RIGHT))
		{
			eulers.x -= rotSpeed * deltaTime;
			mFloor->SetRotation(eulers);
		}
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_UP))
		{
			eulers.z += rotSpeed * deltaTime;
			mFloor->SetRotation(eulers);
		}
		if (InputHandler::GetInstance()->KeyHasPressed(GLFW_KEY_DOWN))
		{
			eulers.z -= rotSpeed * deltaTime;
			mFloor->SetRotation(eulers);
		}
	}

	void StackOfBoxesDemo::CreateStackOfBoxes()
	{
		int rows = 5;
		int cols = 6;
		float startY = 50.0f;
		float boxDim = 20.0f;
		float offsetX = ((cols - 1) / 2.0f) * boxDim;

		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				float x = (c * boxDim) - offsetX;
				float y = startY + r * boxDim;
				float z = 0.0f;

				MaterialInfo info = LoadMaterialInfo("crate");
				info.mRoughness = 1.0f;
				info.mMetallic = 0.0f;
				info.mTiling = 1;
				info.mBaseColor = Random::GetInstance()->GetPointOnUnitSphere();
				AddBox(glm::vec3(x, y, z), glm::vec3(boxDim), glm::vec3(glm::radians(0.0f), 0.0f, 0.0f), 30.0f,
				       info, 0);
			}
		}
	}

	void StackOfBoxesDemo::ImGuiProc()
	{
		EkScene::ImGuiProc();

		if (ImGui::Begin("Stack Of Boxes", nullptr,
		                 ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Press H to generate Stack Of Boxes");
			ImGui::Text("Press UP/DOWN/LEFT/RIGHT to rotate the floor");
			ImGui::Text("Left Mouse click to shoot a sphere");
			ImGui::Text("Spacebar to reset the scene");
			ImGui::Text("PRESS ESCAPE TO GO BACK TO DEMO OPTIONS");
			ImGui::End();
		}
	}

#ifdef EKDEBUG
	void StackOfBoxesDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);
	}


#endif
} // namespace Eklavya
