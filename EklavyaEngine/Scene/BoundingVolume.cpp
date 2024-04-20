#include "BoundingVolume.h"

namespace Eklavya {
Bound::Bound()
    : mMin(std::numeric_limits<float>::max()),
      mMax(std::numeric_limits<float>::min()) {
  mPoints[0] = glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
  mPoints[1] = glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
  mPoints[2] = glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
  mPoints[3] = glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);

  mPoints[4] = glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
  mPoints[5] = glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
  mPoints[6] = glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
  mPoints[7] = glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
}

Bound::~Bound() {}
} // namespace Eklavya
