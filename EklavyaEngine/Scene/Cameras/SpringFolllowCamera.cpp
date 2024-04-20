#include "SpringFolllowCamera.h"
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Timer.h"
#include <GLFW/glfw3.h>
#include "Helpers.h"
#include "Renderer/DebugRenderer.hpp"
#include "EkScene.h"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include "CoreUtils/Logger.h"

namespace Eklavya
{
  SpringFollowCamera::SpringFollowCamera(CameraParams options)
      : ICamera(options)
  {
  }

  SpringFollowCamera::~SpringFollowCamera() {}

  void SpringFollowCamera::Update(float dt)
  {
    if (mTarget != nullptr)
      {

        glm::vec3 localArmsLength =
            glm::vec3(mTarget->Rotation() * glm::vec4(mArmsLength, 1.0f));
        glm::vec3 localTargetOffset = glm::vec3(mTarget->Rotation() * glm::vec4(mTargetOffset, 1.0f));
        glm::vec3 targetPosition = mTarget->Position();

        mPosition = targetPosition + localArmsLength;

        mView = glm::lookAt(mPosition, targetPosition + localTargetOffset,
                            glm::vec3(0.0f, 1.0f, 0.0f));
      }

    ICamera::Update(dt);
  }

#ifdef EKDEBUG
  void SpringFollowCamera::DebugDraw(Renderer::DebugRenderer &debugRenderer)
  {
    ICamera::DebugDraw(debugRenderer);
  }
#endif

} // namespace Eklavya
