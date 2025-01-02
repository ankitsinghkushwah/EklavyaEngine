//
//  BaseDemoScene.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 19/09/22.
//

#include "StackOfBoxesDemo.hpp"
#include <InputHandler.h>
#include "RenderComponent.hpp"
#include "EkActor.h"
#include "AssetManager.h"
#include "Material.h"
#include "imgui.h"
#include "Random.h"
#include "EkPhysics/EkBody.h"
#include "EkPhysics/Collider.h"
#include <memory>
#include "FreeLookCamera.h"
#include "CarComponent.hpp"
#include "Animation.h"
#include "glfw/glfw3.h"

using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{

	StackOfBoxesDemo::StackOfBoxesDemo(Director* pDirector) : MainEntryScene(pDirector), mEngineLoopSound("shotgun.wav")
	{
		PreloadTextures();

		CreateStage();
		mAudio.Load({mEngineLoopSound});
		mAudio.GetSound().setRelativeToListener(false);
		mAudio.GetSound().setVolume(100.0f);
		mAudio.GetSound().setMinDistance(400.0f);
		mAudio.GetSound().setAttenuation(5.0f);
		mAudio.GetSound().setLoop(false);
	}

	StackOfBoxesDemo::~StackOfBoxesDemo()
	{
	}

	void StackOfBoxesDemo::OnMouseAction(int key, int action)
	{
		if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
		{
			mRayStart = CurrentCamera()->Position();
			mRayDirection = glm::normalize(CurrentCamera()->Forward());

			if (glm::dot(mRayDirection, glm::vec3(0.0f, 1.0f, 0.0f)) > 0.0f)
			{
				mRayDirection *= -1.0f;
			}
			mRayRange = 30000.0f;

			mLastCastHitResult = mPhysicsWorld->RayCast(mRayStart, mRayDirection, mRayRange, -1);

			if (mLastCastHitResult.success)
			{
				mLastCastHitResult.body->ApplyImpulseAtPoint(glm::normalize(CurrentCamera()->Forward()) * 1000.0f, mLastCastHitResult.position);
			}
		}
	}

	void StackOfBoxesDemo::PreloadTextures()
	{
		std::string ext = "png";

		std::vector<std::string> texturesList = {"grid", "crate", "grass", "floor"};

		for (auto& name : texturesList)
		{
			std::string folder = "pbr/" + name + "/";

			AssetManager::GetInstance().LoadTexture(folder + "albedo", ext);
			AssetManager::GetInstance().LoadTexture(folder + "normal", ext);
			AssetManager::GetInstance().LoadTexture(folder + "ao", ext);
			AssetManager::GetInstance().LoadTexture(folder + "roughness", ext);
			AssetManager::GetInstance().LoadTexture(folder + "metallic", ext);
		}
	}

	MaterialInfo StackOfBoxesDemo::LoadMaterialInfo(const std::string& file, std::string ext)
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
		MaterialInfo info;
		info.mBaseColor = glm::vec4(0.5, 0.0f, 0.0f, 0.6f);
		info.mRoughness = .5f;
		info.mTiling = 10;
		info.mBaseColor = glm::vec3(0.0f, 0.6f, 0.2f);

		float area_extent = 300;
		float floorScaleY = 10.0f;

		//CreateSphere(glm::vec3(0.0f,-50.0f,0.0f), 100.0f, FLT_MAX, info);
		CreateCube(glm::vec3(0.0f), glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(), FLT_MAX, info, 0);

		//    CreateCube(glm::vec3(0.0f,50.0f,0.0f),
		//               glm::vec3(30.0f), glm::vec3(0.0,0.0f,0.0f),
		//               FLT_MAX, info, 0);

		info.mBaseColor = glm::vec3(0.0f, 0.4, 0.2f);
	}

	void StackOfBoxesDemo::OnKeyAction(int key, int action)
	{
		if (key == GLFW_KEY_H && action == GLFW_PRESS)
			CreateStackOfBoxes();
	}

	void StackOfBoxesDemo::CreateStackOfBoxes()
	{
		int   rows = 5;
		int   cols = 5;
		float startY = 100.0f;
		float boxDim = 20.0f;

		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				float x = c * boxDim;
				float y = startY + r * boxDim;
				float z = 0.0f;

				MaterialInfo info = LoadMaterialInfo("crate");
				info.mRoughness = 1.0f;
				info.mMetallic = 0.0f;
				info.mTiling = 1;
				info.mBaseColor = Random::GetInstance()->GetPointOnUnitSphere();
				CreateCube(glm::vec3(x, y, z), glm::vec3(boxDim), glm::vec3(glm::radians(0.0f), 0.0f, 0.0f), 100.0f, info, 0);

				//CreateSphere(glm::vec3(x, y, z), boxDim, 20.0f, info);
			}
		}
	}

#ifdef EKDEBUG
	void StackOfBoxesDemo::DebugDraw(Renderer::DebugRenderer& debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);

		glm::vec3 endPoint = mLastCastHitResult.success ? mLastCastHitResult.position : mRayStart + mRayDirection * mRayRange;
		debugRenderer.DrawLine(mRayStart, endPoint, glm::vec4(0.0f, 0.0f, 0.0f, .7f), .2f);
		debugRenderer.DrawSphere(endPoint, 2.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
	}
	void StackOfBoxesDemo::ImGuiProc()
	{
		MainEntryScene::ImGuiProc();
	}
#endif

} // namespace Eklavya
