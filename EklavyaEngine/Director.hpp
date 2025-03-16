//
//  Engine.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 26/05/2023.
//

#ifndef Engine_hpp
#define Engine_hpp

#include <GLFWGame.h>
#include <Scene/EkScene.h>
#include <MainEntryScene.hpp>
#include <string>

namespace Eklavya
{
	// forward declarations
	class EkScene;

	class Director : public GLFWGame
	{
	public:
		Director(const std::string &title, uint32_t width, uint32_t height, bool fullScreen);

		~Director();

		void LoadAssets();

		void SetScene(MainEntryScene &sceneImpl);

		void SetPhysicsTickRate(float physicsTickRate)
		{
			mPhysicsTickRate = physicsTickRate;
		}

		float PhysicsTickRate()
		{
			return mPhysicsTickRate;
		}

		template<typename SceneType>
		SceneType *CreateNewScene()
		{
			return new SceneType(this);
		}

		void Tick() override;

		void ImGuiProc() override;

		void OnKeyAction(int pKey, int pAction) override;

		void OnError(int pError, const char *pDesc) override;

		void OnResize(int newWidth, int newHeight) override;

		void OnMouseAction(int pKey, int pAction) override;

		void OnCursorMoved(double pX, double pY) override;

		void OnJoystickStateChange(int, int) override;

	private:
		MainEntryScene *mCurrentScene = nullptr;
		float mPhysicsTickRate = 1.0f / 60.0f;
		float mTimeSinceStart = 0.0f;
		float mTimeScale = 1.0f;
	};
} // namespace Eklavya

#endif /* Engine_hpp */
