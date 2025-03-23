#include "FreeLookCamera.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <Timer.h>
#include <GLFW/glfw3.h>
#include <Helpers.h>
#include <Renderer/DebugRenderer.hpp>
#include <Scene/EkScene.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <CoreUtils/Logger.h>

namespace Eklavya
{
	FreeLookCamera::FreeLookCamera(CameraParams options) :
		ICamera(options), mSenstivity(.005), mSpeed(200.0f)
	{
		mPosition = glm::vec3(0.0f, 100.0f, 200.0f);
	}

	void FreeLookCamera::OnCursorMove(double x, double y)
	{
		if (mEnabled == false)
		{
			return;
		}

		glm::vec2 dragDelta = InputHandler::GetInstance()->GetMouseDragDelta();
		float xOffset = dragDelta.x;
		float yOffset = dragDelta.y;

		xOffset *= mSenstivity;
		yOffset *= mSenstivity;

		mYaw += xOffset;
		mPitch -= yOffset;

		float max = .999;

		if (mPitch > max)
		{
			mPitch = max;
		}
		else if (mPitch < -max)
		{
			mPitch = -max;
		}

		mForward.x = cos(mYaw) * cos(mPitch);
		mForward.y = sin(mPitch);
		mForward.z = sin(mYaw) * cos(mPitch);

		mForward = glm::normalize(mForward);
	}

	FreeLookCamera::~FreeLookCamera() {}

	void FreeLookCamera::Update(float dt)
	{
		mView = glm::lookAt(mPosition, mPosition + mForward, glm::vec3(0.0f, 1.0f, 0.0f));

		ICamera::Update(dt);
	}

	void FreeLookCamera::PollKeyAction()
	{
		float dt = Timer::GetInstance().GetFrameTime();

		if (IsKeyPressed(GLFW_KEY_W))
			Move(EDirection::UP, dt);
		if (IsKeyPressed(GLFW_KEY_S))
			Move(EDirection::DOWN, dt);
		if (IsKeyPressed(GLFW_KEY_A))
			Move(EDirection::LEFT, dt);
		if (IsKeyPressed(GLFW_KEY_D))
			Move(EDirection::RIGHT, dt);
	}

	void FreeLookCamera::OnKeyAction(int key, int action)
	{
		ICamera::OnKeyAction(key, action);
	}

	void FreeLookCamera::MoveForController(float dt)
	{
		float rightX = -InputHandler::GetInstance()->GetAxis(GLFW_GAMEPAD_AXIS_RIGHT_X);
		float rightY = -InputHandler::GetInstance()->GetAxis(GLFW_GAMEPAD_AXIS_RIGHT_Y);

		float speed = mSpeed * dt;

		mPosition += rightX * speed * glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), mForward);
		mPosition += rightY * speed * mForward;
	}

	void FreeLookCamera::Move(EDirection direction, float dt)
	{
		float speed = mSpeed * dt;

		switch (direction)
		{
			case EDirection::LEFT:
				mPosition += speed * glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), mForward);
				break;
				break;
			case EDirection::RIGHT:
				mPosition += speed * glm::cross(mForward, glm::vec3(0.0f, 1.0f, 0.0f));
				break;
			case EDirection::UP:
				mPosition += speed * mForward;
				break;
			case EDirection::DOWN:
				mPosition += speed * -mForward;
				break;
		}
	}

#ifdef EKDEBUG
	void FreeLookCamera::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		ICamera::DebugDraw(debugRenderer);
	}
#endif
} // namespace Eklavya
