#ifndef _INC_BOUNDING_VOLUME_H_
#define _INC_BOUNDING_VOLUME_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <memory>

namespace Eklavya {
struct Bound {
  Bound();
  ~Bound();

  glm::vec3 Extents() const { return mMax - mMin; }

  glm::vec3 mMin;
  glm::vec3 mMax;
  glm::vec4 mPoints[8];
};
} // namespace Eklavya
#endif
