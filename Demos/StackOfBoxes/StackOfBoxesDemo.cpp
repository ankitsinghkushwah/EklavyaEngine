//
//  StackOfBoxesDemo.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "StackOfBoxesDemo.hpp"
#include <InputHandler.h>
#include "AssetManager/AssetManager.h"
#include "imgui/imgui.h"
#include "Random.h"
#include "EkPhysics/EkBody.h"
#include "Director.hpp"
#include "GLFW/glfw3.h"
#include <algorithm>

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	namespace
	{
		const glm::vec3 kFloorSize(1000.0f, 30.0f, 1000.0f);
		const float kFloorTiltSpeed = glm::radians(5.0f);
		const float kSphereMass = 60.0f;
		const float kSphereSpeed = 300.0f;

		MaterialInfo LoadPbrMaterial(const std::string &name)
		{
			const std::string folder = "pbr/" + name + "/";
			AssetManager &assets = AssetManager::GetInstance();

			for (const char *map: {"albedo", "normal", "ao", "roughness", "metallic"})
			{
				assets.LoadTexture(folder + map, "png");
			}

			MaterialInfo info;
			info.mBaseMap = assets.GetAsset<Texture2D>(folder + "albedo");
			info.mNormalMap = assets.GetAsset<Texture2D>(folder + "normal");
			info.mAOMap = assets.GetAsset<Texture2D>(folder + "ao");
			info.mRoughMap = assets.GetAsset<Texture2D>(folder + "roughness");
			info.mMetalMap = assets.GetAsset<Texture2D>(folder + "metallic");
			return info;
		}
	} // namespace

	StackOfBoxesDemo::StackOfBoxesDemo(Director &pDirector) :
		MainEntryScene(pDirector), mPopSound("StackOfBoxesDemo/pop.mp3")
	{
		LoadMaterials();
		CreateFloor();
		SetFlyMode(false);

		mAudio.Load({mPopSound});
		sf::Sound &sound = mAudio.GetSound();
		sound.setRelativeToListener(false);
		sound.setVolume(100.0f);
		sound.setMinDistance(400.0f);
		sound.setAttenuation(5.0f);
		sound.setLoop(false);
	}

	StackOfBoxesDemo::~StackOfBoxesDemo() {}

	void StackOfBoxesDemo::LoadMaterials()
	{
		mFloorMaterial = LoadPbrMaterial("grid");

		mCrateMaterial = LoadPbrMaterial("crate");
		mCrateMaterial.mRoughness = 1.0f;
		mCrateMaterial.mMetallic = 0.0f;
		mCrateMaterial.mTiling = 1;
	}

	void StackOfBoxesDemo::CreateFloor()
	{
		EkActor *floor = AddBox(glm::vec3(0.0f), kFloorSize, glm::vec3(0.0f), FLT_MAX, mFloorMaterial, STATIC);
		mFloor = floor->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
	}

	void StackOfBoxesDemo::SetFlyMode(bool fly)
	{
		mFlyMode = fly;
		mDirector.HideMouse(fly);
		CurrentCamera()->SetEnabled(fly);
	}

	void StackOfBoxesDemo::BuildStack()
	{
		if (mShape == EStackShape::Grid)
			BuildGrid();
		else
			BuildPyramid();
	}

	void StackOfBoxesDemo::BuildGrid()
	{
		const float step = mBoxSize + mGap;
		const float baseY = kFloorSize.y * 0.5f + mDropHeight + mBoxSize * 0.5f;

		for (int row = 0; row < mRows; ++row)
		{
			for (int col = 0; col < mColumns; ++col)
			{
				for (int d = 0; d < mDepth; ++d)
				{
					const float x = (col - (mColumns - 1) * 0.5f) * step;
					const float z = (d - (mDepth - 1) * 0.5f) * step;
					SpawnBox(glm::vec3(x, baseY + row * step, z));
				}
			}
		}
	}

	void StackOfBoxesDemo::BuildPyramid()
	{
		const float step = mBoxSize + mGap;
		const float baseY = kFloorSize.y * 0.5f + mDropHeight + mBoxSize * 0.5f;

		for (int layer = 0; layer < mPyramidBase; ++layer)
		{
			const int columns = mPyramidBase - layer;
			const int depth = std::max(mPyramidDepth - layer, 1);

			for (int col = 0; col < columns; ++col)
			{
				for (int d = 0; d < depth; ++d)
				{
					const float x = (col - (columns - 1) * 0.5f) * step;
					const float z = (d - (depth - 1) * 0.5f) * step;
					SpawnBox(glm::vec3(x, baseY + layer * step, z));
				}
			}
		}
	}

	void StackOfBoxesDemo::SpawnBox(const glm::vec3 &position)
	{
		MaterialInfo info = mCrateMaterial;
		info.mBaseColor = glm::vec4(Random::GetInstance()->GetPointOnUnitSphere(), 1.0f);
		EkActor *box = AddBox(position, glm::vec3(mBoxSize), glm::vec3(0.0f), mBoxMass, info, 0);
		mSpawned.push_back(box->ID());
	}

	void StackOfBoxesDemo::ShootSphere()
	{
		const glm::vec3 forward = glm::normalize(CurrentCamera()->Forward());
		const glm::vec3 position = CurrentCamera()->Position() + forward * 10.0f;

		MaterialInfo info;
		info.mRoughness = 0.0f;
		info.mMetallic = 0.0f;
		info.mBaseColor = glm::vec4(Random::GetInstance()->GetPointOnUnitSphere() * 0.1f + glm::vec3(0.1f), 1.0f);

		const float radius = Random::GetInstance()->Real(0.4f, 2.0f) * 15.0f;
		EkActor *sphere = AddSphere(position, radius, kSphereMass, info, -1);
		sphere->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID)->SetVelocity(forward * kSphereSpeed);
		mSpawned.push_back(sphere->ID());

		mAudio.Play(mPopSound, false);
	}

	void StackOfBoxesDemo::ClearSpawned()
	{
		for (EkActorID id: mSpawned)
		{
			RemoveActor(id);
		}
		mSpawned.clear();
	}

	void StackOfBoxesDemo::ResetScene()
	{
		ClearSpawned();
		mFloor->SetRotation(glm::vec3(0.0f));
	}

	void StackOfBoxesDemo::TiltFloor(float deltaTime)
	{
		InputHandler *input = InputHandler::GetInstance();
		glm::vec3 delta(0.0f);

		if (input->KeyHasPressed(GLFW_KEY_LEFT))
			delta.x += 1.0f;
		if (input->KeyHasPressed(GLFW_KEY_RIGHT))
			delta.x -= 1.0f;
		if (input->KeyHasPressed(GLFW_KEY_UP))
			delta.z += 1.0f;
		if (input->KeyHasPressed(GLFW_KEY_DOWN))
			delta.z -= 1.0f;

		if (delta == glm::vec3(0.0f))
			return;

		const glm::vec3 eulers = glm::eulerAngles(mFloor->GetOwner().Transform().Rotation());
		mFloor->SetRotation(eulers + delta * kFloorTiltSpeed * deltaTime);
	}

	void StackOfBoxesDemo::OnKeyAction(int key, int action)
	{
		MainEntryScene::OnKeyAction(key, action);

		if (action != GLFW_PRESS)
			return;

		if (key == GLFW_KEY_TAB)
			SetFlyMode(!mFlyMode);
		else if (key == GLFW_KEY_H)
			BuildStack();
		else if (key == GLFW_KEY_SPACE)
			ResetScene();
	}

	void StackOfBoxesDemo::OnMouseAction(int key, int action)
	{
		if (mFlyMode && key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
			ShootSphere();
	}

	void StackOfBoxesDemo::Tick(float deltaTime)
	{
		MainEntryScene::Tick(deltaTime);
		TiltFloor(deltaTime);
	}

	void StackOfBoxesDemo::ImGuiProc()
	{
		EkScene::ImGuiProc();

		if (ImGui::Begin("Stack Of Boxes", nullptr, ImGuiWindowFlags_AlwaysAutoResize))
		{
			int shape = static_cast<int>(mShape);
			ImGui::RadioButton("Grid", &shape, static_cast<int>(EStackShape::Grid));
			ImGui::SameLine();
			ImGui::RadioButton("Pyramid", &shape, static_cast<int>(EStackShape::Pyramid));
			mShape = static_cast<EStackShape>(shape);

			int boxCount = 0;
			if (mShape == EStackShape::Grid)
			{
				ImGui::SliderInt("Columns", &mColumns, 1, 15);
				ImGui::SliderInt("Rows", &mRows, 1, 20);
				ImGui::SliderInt("Depth", &mDepth, 1, 10);
				boxCount = mColumns * mRows * mDepth;
			}
			else
			{
				ImGui::SliderInt("Base", &mPyramidBase, 1, 15);
				ImGui::SliderInt("Depth", &mPyramidDepth, 1, 15);
				for (int layer = 0; layer < mPyramidBase; ++layer)
					boxCount += (mPyramidBase - layer) * std::max(mPyramidDepth - layer, 1);
			}

			ImGui::SliderFloat("Box size", &mBoxSize, 5.0f, 50.0f, "%.1f");
			ImGui::SliderFloat("Box mass", &mBoxMass, 1.0f, 200.0f, "%.1f");
			ImGui::SliderFloat("Gap", &mGap, 0.0f, 10.0f, "%.1f");
			ImGui::SliderFloat("Drop height", &mDropHeight, 0.0f, 200.0f, "%.1f");
			ImGui::Text("Boxes: %d", boxCount);

			if (ImGui::Button("Build (H)"))
				BuildStack();
			ImGui::SameLine();
			if (ImGui::Button("Clear"))
				ClearSpawned();
			ImGui::SameLine();
			if (ImGui::Button("Reset (Space)"))
				ResetScene();

			ImGui::Separator();
			ImGui::Text("Bodies spawned: %d", static_cast<int>(mSpawned.size()));
			ImGui::Text("TAB: %s", mFlyMode ? "fly camera (click to shoot)" : "edit (mouse free)");
			ImGui::Text("Arrow keys: tilt the floor");
			ImGui::Text("Escape: back to demo options");
		}
		ImGui::End();
	}
} // namespace Eklavya
