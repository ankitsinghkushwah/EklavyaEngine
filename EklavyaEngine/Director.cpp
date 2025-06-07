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

		PushScene(new MainEntryScene(*this));
	}

	Director::~Director()
	{
		while (mSceneStack.empty() == false)
		{
			mSceneStack.top()->DeleteDeferredDeadActors();
			mSceneStack.pop();
		}
	}

	void Director::PushScene(MainEntryScene *sceneImpl)
	{
		if (mSceneStack.size() < 2)
		{
			mSceneStack.push(sceneImpl);
		}
	}

	void Director::LoadAssets()
	{
		AssetManager::GetInstance().LoadShader("standard_brdf");
		AssetManager::GetInstance().LoadShader("skybox");
		AssetManager::GetInstance().LoadShader("main_output");
		AssetManager::GetInstance().LoadShader("shadow_map");
		AssetManager::GetInstance().LoadShader("anim_model");

		SHARED_SHADER program = AssetManager::GetInstance().LoadShader("unlit_solids");
	}

	void Director::Tick()
	{
		Timer::GetInstance().Update();

		float frameTime = Timer::GetInstance().GetFrameTime();

		InputHandler::GetInstance()->Update(frameTime);


		if (mSceneStack.top() != nullptr)
		{
			mSceneStack.top()->Tick(frameTime * mTimeScale);
			mSceneStack.top()->FixedTick(mPhysicsTickRate * mTimeScale);
			mSceneStack.top()->Draw();
			mSceneStack.top()->DeleteDeferredDeadActors();
		}
	}

	void Director::ImGuiProc()
	{
		GLFWGame::ImGuiProc();

		InputHandler::GetInstance()->ImGuiProc();
		if (mSceneStack.top())
		{
			mSceneStack.top()->ImGuiProc();
		}
	}

	void Director::OnKeyAction(int pKey, int pAction)
	{
		InputHandler::GetInstance()->OnKeyAction(pKey, pAction);


		if (pKey == GLFW_KEY_ESCAPE && pAction == GLFW_PRESS)
		{
			if (mSceneStack.size() > 1)
			{
				mSceneStack.top()->Cleanup();
				delete mSceneStack.top();
				mSceneStack.pop();
				mSceneStack.top()->OnResume();
				return;
			}

			CloseWindow();
		}
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
