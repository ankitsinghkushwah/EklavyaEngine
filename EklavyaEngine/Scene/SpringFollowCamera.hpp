//
//  SpringFollowCamera.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 09/07/2023.
//

#ifndef SpringFollowCamera_hpp
#define SpringFollowCamera_hpp

#include <stdio.h>
#include "TransformComponent.h"
#include <memory>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Eklavya
{
	struct SpringFollowCamera
	{
		void UpdateCamera(float dt);

		void Init(float fov, float ratio, float nearDist, float farDist);

		glm::mat4 GetView() const
		{
			return mView;
		}

		glm::mat4 GetProjection() const
		{
			return mProjection;
		}

		float               fov = 45.0f;
		glm::vec3           mArmLengths;
		glm::vec3           length;
		glm::vec3           mFront;
		glm::vec3           mPosition;
		glm::vec3           mLocalPosition;
		glm::vec3           mLastTargetPosition;
		glm::mat4           mView;
		glm::mat4           mProjection;
		TransformComponent* mTarget = nullptr;

		void ResumeCamera()
		{
		}

		void PauseCamera()
		{
		}

		float mSpringStrength = 100;
		float mSpringDamping = 50.0f;

	  private:
		glm::vec3 mVelocity;
		glm::vec3 mAcceleration;
		glm::vec3 mForces;
	};
} // namespace Eklavya

#endif /* SpringFollowCamera_hpp */
