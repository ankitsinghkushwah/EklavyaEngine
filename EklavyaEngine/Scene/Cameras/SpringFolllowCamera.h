#ifndef INC_SPRING_FOLLOW_CAMERA_H
#define INC_SPRING_FOLLOW_CAMERA_H

#include <memory>
#include <glm/glm.hpp>
#include "UserInputListener.h"
#include "ICamera.hpp"
#include "EkActor.h"

namespace Eklavya
{
	class TransformComponent;

	class SpringFollowCamera : public ICamera
	{
	  public:
		SpringFollowCamera(CameraParams options);
		~SpringFollowCamera();
		void Update(float deltaTime) override;

		void SetTarget(TransformComponent* actor)
		{
			mTarget = actor;
      mCameraRotation = actor->Rotation();
		}

		void SetArmsLength(glm::vec3 armsLength)
		{
			mArmsLength = armsLength;
		}

		void SetTargetOffset(glm::vec3 targetOffset)
		{
			mTargetOffset = targetOffset;
		}

	  private:
		glm::vec3           mArmsLength;
		glm::vec3           mTargetOffset;
		TransformComponent* mTarget = nullptr;
		float               mSpeed = 0.0f;
    float mAngCatchupSpeed = 0.0f;
    float mPosCatchupSpeed = 0.0f;
    glm::quat mCameraRotation;

#ifdef EKDEBUG
		void DebugDraw(Renderer::DebugRenderer& debugRenderer) override;
#endif
	};
} // namespace Eklavya

#endif
