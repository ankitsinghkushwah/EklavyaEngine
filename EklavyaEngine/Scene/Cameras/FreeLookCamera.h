#ifndef _INC_DEBUG_CAMERA_H_
#define _INC_DEBUG_CAMERA_H_

#include <memory>
#include <glm/glm.hpp>
#include <UserInputListener.h>
#include "ICamera.hpp"

namespace Eklavya
{
	class FreeLookCamera : public ICamera
	{
	  public:
		enum EDirection
		{
			LEFT,
			RIGHT,
			UP,
			DOWN
		};

		FreeLookCamera(CameraParams options);
		~FreeLookCamera();
		void OnCursorMove(double x, double y) override;
		void PollKeyAction() override;
		void OnKeyAction(int key, int action) override;
		void Update(float deltaTime) override;
		void SetSenstivity(float senstivity)
		{
			mSenstivity = senstivity;
		}
		void SetEnabled(bool flag)
		{
			mEnabled = flag;
		}

	  private:
		void Move(EDirection direction, float dt);
		void MoveForController(float dt);

	  private:
		float mYaw = 0.0f;
		float mPitch = 0.0f;
		float mSenstivity = 0.005f;
		float mSpeed = 0.0f;
		bool  mEnabled = true;

#ifdef EKDEBUG
		void DebugDraw(Renderer::DebugRenderer& debugRenderer) override;
#endif
	};
} // namespace Eklavya

#endif
