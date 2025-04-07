//
//  ICamera.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 31/03/2024.
//

#include "ICamera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <cstdio>
#include <GLFW/glfw3.h>

#include <Renderer/DebugRenderer.hpp>


namespace Eklavya
{
	ICamera::ICamera(CameraParams options) :
		UserInputListener(), mPosition(0.0f), mForward(0.0f, 0.0f, -1.0f), mView(1.0f), mProjection(1.0f)
	{
		mProjection = glm::perspective(options.fov, options.ratio, options.near, options.far);
		mFrustum.Init(options.fov, options.ratio, options.near, options.far);
	}

	void ICamera::Update(float deltaTime)
	{
		if (mDebug == false)
		{
			mFrustum.UpdatePlanes(mProjection, mView);
		}
	}

	void ICamera::OnKeyAction(int key, int action)
	{
		if (key == GLFW_KEY_G && action == GLFW_PRESS)
		{
			mDebug = !mDebug;

			mFrustum.OnDebugUpdate(mView);
		}
	}

	void ICamera::DebugDraw(Renderer::DebugRenderer &debugRenderer)
	{
		if (mDebug)
			debugRenderer.DrawFrustum(mFrustum);
	}
} // namespace Eklavya
