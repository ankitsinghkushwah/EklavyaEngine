//
// Created by Ankit Singh Kushwah on 16/03/2025.
//

#include "InertiaTensorTorqueTestDemo.h"
#include "Components/RenderComponent.hpp"
#include "AssetManager/AssetManager.h"
#include "Random.h"
#include "EkPhysics/Collider.h"
#include "GLFW/glfw3.h"


using namespace Eklavya::Asset;
using namespace Eklavya::Renderer;
using namespace Eklavya::Physics;

namespace Eklavya
{
	InertiaTensorTorqueTestDemo::InertiaTensorTorqueTestDemo(Director &pDirector) :
		MainEntryScene(pDirector)
	{
		PreloadTextures();
		CreateStage();
	}

	InertiaTensorTorqueTestDemo::~InertiaTensorTorqueTestDemo() {}


	void InertiaTensorTorqueTestDemo::PreloadTextures()
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

	MaterialInfo InertiaTensorTorqueTestDemo::LoadMaterialInfo(const std::string &file, std::string ext)
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

	void InertiaTensorTorqueTestDemo::CreateStage()
	{
		MaterialInfo info = LoadMaterialInfo("grid");
		info.mTiling = 100;
		info.mRoughness = 1.0f;
		info.mMetallic = 0.0f;

		float area_extent = 2000;
		float floorScaleY = 10.0f;


		CreateCube(glm::vec3(0.0f), glm::vec3(area_extent, floorScaleY, area_extent), glm::vec3(), FLT_MAX, info, 0);


		MaterialInfo crate = LoadMaterialInfo("crate");


		mBoxScale = glm::vec3(100.0f);
		mBoxMass = 100.0f;
		auto actor = CreateCube(glm::vec3(0.0f, 100.0f, 0.0f), mBoxScale, glm::vec3(), mBoxMass, crate, 1);
		mBoxBody = actor->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
		mBoxBody->SetGravityScale(0.0f);
		mBoxBody->SetFreezePositionFlags(glm::vec3(0.0f, 0.0f, 0.0f));
	}

	void InertiaTensorTorqueTestDemo::OnKeyAction(int key, int action)
	{
		MainEntryScene::OnKeyAction(key, action);

		if (key == GLFW_KEY_R && action == GLFW_PRESS)
		{
			RemoveActor(mBoxBody->GetOwner().ID(), true);
			MaterialInfo crate = LoadMaterialInfo("crate");
			auto actor = CreateCube(glm::vec3(0.0f, 100.0f, 0.0f), mBoxScale, glm::vec3(), mBoxMass, crate, 1);
			mBoxBody = actor->GetComponent<EkBody>(CoreComponentIds::RIGIDBODY_COMPONENT_ID);
			mBoxBody->SetGravityScale(0.0f);
			mBoxBody->SetFreezePositionFlags(glm::vec3(0.0f, 0.0f, 0.0f));
			std::string boxName;
			std::ostringstream oss;
			oss << "Box, " << ++mBoxIDCounter;
			boxName = oss.str();
			actor->SetName(boxName);
		}

		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS)
		{
			mForceAppliedText.clear();
		}
	}

	void InertiaTensorTorqueTestDemo::OnMouseAction(int key, int action)
	{
		MainEntryScene::OnMouseAction(key, action);

		if (key == GLFW_MOUSE_BUTTON_1 && action == GLFW_PRESS)
		{
			mImpulseTargetID = ++mImpulseTargetID % 3;


			glm::vec3 position;
			std::string side;

			switch (mImpulseTargetID)
			{
				case 0:

					position = glm::vec3(.9f, 0.0f, 0.0f);
					side = "left";
					break;
				case 1:
					position = glm::vec3(0.0f, 0.0f, 0.0f);
					side = "middle";
					break;
				case 2:
					position = glm::vec3(-.9f, 0.0f, 0.0f);
					side = "right";
					break;
			}

			position *= mBoxScale * 0.5f;
			position += mBoxBody->GetPosition();
			position.z += 150.0f;

			mRayDirection = glm::vec3(0.0f, 0.0f, -1.0f);
			mRayStart = position;

			mRayRange = 3000.0f;

			mLastCastHitResult = GetPhysics().RayCast(mRayStart, mRayDirection, mRayRange, -1);

			if (mLastCastHitResult.success)
			{
				glm::vec3 prevW = mBoxBody->GetAngularVel();

				mLastCastHitResult.body->ApplyImpulseAtPoint(
					mRayDirection * mImpulseForce,
					mLastCastHitResult.position);

				glm::vec3 currW = mBoxBody->GetAngularVel();

				std::ostringstream out;
				glm::vec3 delta = currW - prevW;
				out << " Delta W ( " << side << " ) : " << delta.x << ", " << delta.y << ", " << delta.z << "\n";
				mForceAppliedText += out.str();
			}
		}
	}

#ifdef EKDEBUG
	void InertiaTensorTorqueTestDemo::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		MainEntryScene::DebugDraw(debugRenderer);

		MainEntryScene::DebugDraw(debugRenderer);

		glm::vec3 endPoint = mLastCastHitResult.success
			                     ? mLastCastHitResult.position
			                     : mRayStart + mRayDirection * mRayRange;
		debugRenderer.DrawLine(mRayStart,
		                       endPoint,
		                       mLastCastHitResult.success
			                       ? glm::vec4(0.0f, 0.6f, 0.0f, 1.0f)
			                       : glm::vec4(0.0f, 0.0f, 0.0f, .7f),
		                       mLastCastHitResult.success ? 0.5f : .3f);
		debugRenderer.DrawSphere(endPoint, 2.0f, glm::vec4(1.0f, 0.0f, 0.0f, 0.5f));
	}

	void InertiaTensorTorqueTestDemo::ImGuiProc()
	{
		ImGui::Begin("Inertia Torque Test Demo");

		ImGui::InputFloat("Force ", &mImpulseForce);

		ImGui::Text(mForceAppliedText.c_str());

		ImGui::End();
	}
#endif
} // namespace Eklavya
