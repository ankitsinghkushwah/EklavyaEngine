#include "Frustum.h"
#include "VertexArrayObject.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_access.hpp>
#include "Helpers.h"
#include "BoundingVolume.h"

namespace Eklavya
{
  void Frustum::Init(float fov, float ratio, float fNear, float fFar)
  {
    float halfOfFov = fov / static_cast<float>(2);

    mNearHeight = 2 * tan(glm::radians(halfOfFov)) * fNear;
    mNearWidth = mNearHeight * ratio;

    mFarHeight = 2 * tan(glm::radians(halfOfFov)) * fFar;
    mFarWidth = mFarHeight * ratio;

    glm::vec3 forward = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);

    glm::vec3 nc = forward * fNear; // assuming camera is at (0,0,0)
    glm::vec3 fc = forward * fFar;  // assuming camera is at (0,0,0)

#ifdef EKDEBUG
    mPoints[NEAR_CENTER] = nc;
    mPoints[FAR_CENTER] = fc;
    mPoints[NEAR_TOP_LEFT] =
        nc + (up * (mNearHeight / 2.0f)) - (right * (mNearWidth / 2.0f));
    mPoints[NEAR_TOP_RIGHT] = mPoints[NEAR_TOP_LEFT] + (right * mNearWidth);
    mPoints[NEAR_BOTTOM_LEFT] = mPoints[NEAR_TOP_LEFT] - (up * mNearHeight);
    mPoints[NEAR_BOTTOM_RIGHT] =
        mPoints[NEAR_BOTTOM_LEFT] + (right * mNearWidth);

    mPoints[FAR_TOP_LEFT] =
        fc + (up * (mFarHeight / 2.0f)) - (right * (mFarWidth / 2.0f));
    mPoints[FAR_TOP_RIGHT] = mPoints[FAR_TOP_LEFT] + (right * mFarWidth);
    mPoints[FAR_BOTTOM_LEFT] = mPoints[FAR_TOP_LEFT] - (up * mFarHeight);
    mPoints[FAR_BOTTOM_RIGHT] = mPoints[FAR_BOTTOM_LEFT] + (right * mFarWidth);
#endif
  }

  Frustum::~Frustum() {}

#ifdef EKDEBUG
  void Frustum::OnDebugUpdate(const glm::mat4 &view)
  {
    for (int i = 0; i < 10; i++)
      {
        mWorldPoints[i] = glm::inverse(view) * glm::vec4(mPoints[i], 1.0f);
      }
  }
#endif

  void Frustum::UpdatePlanes(const glm::mat4 &projection, const glm::mat4 &view)
  {

    glm::vec4 row1 = glm::row(projection, 0);
    glm::vec4 row2 = glm::row(projection, 1);
    glm::vec4 row3 = glm::row(projection, 2);
    glm::vec4 row4 = glm::row(projection, 3);
    // normals will be pointing inward
    mPlanes[EPlane::LEFT_PLANE] = glm::normalize(row4 + row1) * view;
    mPlanes[EPlane::RIGHT_PLANE] = glm::normalize(row4 - row1) * view;
    mPlanes[EPlane::BOTTOM_PLANE] = glm::normalize(row4 + row2) * view;
    mPlanes[EPlane::TOP_PLANE] = glm::normalize(row4 - row2) * view;
    mPlanes[EPlane::NEAR_PLANE] = glm::normalize(row4 + row3) * view;
    mPlanes[EPlane::FAR_PLANE] = glm::normalize(row4 - row3) * view;
  }

  bool Frustum::Test(const glm::mat4 &transform, const Bound &box) const
  {
      int insidePlaneCounter = 0;
      const glm::vec3 extents = box.Extents();
      for (auto plane : mPlanes) {
          for (int i = 0; i < 8; ++i) {
              glm::vec3 vertex = box.mPoints[i];
              vertex *= extents;
              vertex = transform * glm::vec4(vertex, 1.0f);
              float dist = plane.x * vertex.x + plane.y * vertex.y +
              plane.z * vertex.z + plane.w;
              if (dist >= 0) {
                  insidePlaneCounter++;
                  break;
              }
          }
      }
      if (insidePlaneCounter == 6)
          return true;
      
      return false;
  }
} // namespace Eklavya
