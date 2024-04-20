//
//  DebugHelper.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 25/06/2023.
//
#ifndef DebugHelper_hpp
#define DebugHelper_hpp

#include "VertexArrayObject.h"
#include <glm/glm.hpp>
#include <vector>
#include "Asset.h"
#include "BoundingVolume.h"

namespace Eklavya {
class TransformComponent;
class Frustum;
} // namespace Eklavya

namespace Eklavya::Renderer {
class DebugRenderer {
public:
  DebugRenderer();
  ~DebugRenderer();

  void SetData(glm::mat4 &projection, glm::mat4 &view);

  void DrawLine(glm::vec3 start, glm::vec3 end, glm::vec4 color,
                float thickness);
  void DrawSphere(glm::vec3 center, glm::vec3 rotation, float radius,
                  glm::vec4 color);
  void DrawSphere(glm::vec3 center, glm::vec3 rotation, glm::vec3 extents,
                  glm::vec4 color);

  void DrawBox(glm::vec3 center, glm::vec3 rotation, glm::vec3 extents,
               glm::vec4 color);

  void DrawBound(const glm::mat4 &model, const Bound &bound, glm::vec4 color);

  void DrawPoints(std::vector<const glm::vec3> &points, glm::vec4 color);

  void DrawTransform(TransformComponent &transform);

  void DrawFrustum(const Frustum &frustum);

private:
  glm::mat4 GetModel(glm::vec3 t, const glm::quat &r, glm::vec3 s);
  glm::mat4 mProjection;
  glm::mat4 mView;
  SHARED_SHADER mUnlitSolids = nullptr;
  SHARED_SHADER mWorldPoints = nullptr;

  VertexArrayObject mSphereVAO;
  VertexArrayObject mBoxVAO;
};
} // namespace Eklavya::Renderer

#endif /* DebugHelper_hpp */
