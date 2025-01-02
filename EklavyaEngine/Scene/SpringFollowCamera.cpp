//
//  SpringFollowCamera.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 09/07/2023.
//

#include "SpringFollowCamera.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

namespace Eklavya
{

	void SpringFollowCamera::Init(float fov, float ratio, float nearDist, float farDist)
	{
		mProjection = glm::perspective(fov, ratio, nearDist, farDist);
		mAcceleration = glm::vec3(0.0f);
		mVelocity = glm::vec3(0.0f);
		mForces = glm::vec3(0.0f);

		mView = glm::lookAt(mPosition, mPosition + mFront, glm::vec3(0.0f, 1.0f, 0.0f));
	}

	float strength = 100.0f;
	float damping = 50.0f;

	void SpringFollowCamera::UpdateCamera(float dt)
	{
		if (mTarget == nullptr)
		{
			printf("\n The fuck bro, no target assigned??");
			return;
		}

		glm::vec3 targetPosition = mTarget->Position();

		glm::vec3 localArmsLength = glm::vec3(mTarget->Rotation() * glm::vec4(mArmLengths, 1.0f));

		mPosition = targetPosition + localArmsLength;

		mView = glm::lookAt(mPosition, targetPosition, glm::vec3(0.0f, 1.0f, 0.0f));

		//  mForces = glm::vec3(0.0f);
	}
} // namespace Eklavya
