//
//  Engine.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 26/05/2023.
//

#include <Director.hpp>
#include <Timer.h>
#include <AssetManager/AssetManager.h>
#include <Scene/EkScene.h>
#include "imgui/imgui.h"

namespace Eklavya
{
	Director::Director(const std::string &title, uint32_t width, uint32_t height, bool fullScreen)
	{
		GLFWGame::Initialize(title, width, height, fullScreen);

		LoadAssets();

		mCurrentScene = new MainEntryScene(*this);

		GLFWGame::HideMouse();
	}

	Director::~Director() {}

	void Director::SetScene(MainEntryScene &sceneImpl)
	{
		mCurrentScene = &sceneImpl;
	}

	void Director::LoadAssets()
	{
		AssetManager::GetInstance().LoadShader("standard_brdf");
		AssetManager::GetInstance().LoadShader("skybox");
		AssetManager::GetInstance().LoadShader("main_output");
		AssetManager::GetInstance().LoadShader("shadow_map");
		AssetManager::GetInstance().LoadShader("anim_model");

#ifdef EKDEBUG
		SHARED_SHADER program = AssetManager::GetInstance().LoadShader("unlit_solids");
#endif
	}

	void Director::Tick()
	{
		Timer::GetInstance().Update();

		float frameTime = Timer::GetInstance().GetFrameTime();

		InputHandler::GetInstance()->Update(frameTime);

		if (mCurrentScene != nullptr)
		{
			mCurrentScene->Tick(frameTime * mTimeScale);
			mCurrentScene->FixedTick(mPhysicsTickRate * mTimeScale);
			mCurrentScene->Draw();
			mCurrentScene->Cleanup();
		}
	}

	void Director::ImGuiProc()
	{
		GLFWGame::ImGuiProc();
#ifdef EKDEBUG
		InputHandler::GetInstance()->ImGuiProc();
		if (mCurrentScene)
		{
			mCurrentScene->ImGuiProc();
		}
#endif
	}

	void Director::OnKeyAction(int pKey, int pAction)
	{
		InputHandler::GetInstance()->OnKeyAction(pKey, pAction);

		if (pKey == GLFW_KEY_ESCAPE && pAction == GLFW_PRESS)
			CloseWindow();
	}

	void Director::OnError(int pError, const char *pDesc) {}

	void Director::OnResize(int newWidth, int newHeight)
	{
		// glViewport(0,0,newWidth,newHeight);
	}

	void Director::OnMouseAction(int pKey, int pAction)
	{
		InputHandler::GetInstance()->OnMouseAction(pKey, pAction);
	}

	void Director::OnCursorMoved(double pX, double pY)
	{
		InputHandler::GetInstance()->OnCursorMove(pX, pY);
	}

	void Director::OnJoystickStateChange(int joy, int event)
	{
		InputHandler::GetInstance()->OnControllerStateChanged(joy, event);
	}
} // namespace Eklavya
