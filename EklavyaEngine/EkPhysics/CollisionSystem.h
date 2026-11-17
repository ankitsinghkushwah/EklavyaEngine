//
//  CollisionSystem.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#ifndef CollisionSystem_hpp
#define CollisionSystem_hpp

#include <glm/glm.hpp>
#include <EkPhysics/ContactManifold.h>
#include <array>
#include <vector>

namespace Eklavya::Renderer
{
  class DebugRenderer;
}

namespace Eklavya::Physics
{
  class EkBody;
  class BoxColliderComponent;
  class SphereColliderComponent;

  enum EPhysicsGroupFlag { STATIC, OTHER_COLLIDABLES };

  struct OBB
  {
    glm::vec3 center;
    glm::vec3 halfExtents;
    glm::mat3 rotation;
    glm::mat4 worldTransform;

    glm::vec3 GetPointInLocalSpace(const glm::vec3 &worldPoint) const
    {
      glm::mat4 invWorld = glm::inverse(worldTransform);
      return invWorld * glm::vec4(worldPoint, 1.0f);
    }

    glm::vec3 GetPointInWorldSpace(const glm::vec3 &localPoint) const
    {
      return worldTransform * glm::vec4(localPoint, 1.0f);
    }

    glm::mat3 GetRotationMatrix() const { return rotation; }
  };

  struct Sphere
  {
    glm::vec3 center;
    float radius;
  };

  struct Ray
  {
    glm::vec3 o;
    glm::vec3 d;
    float range;

    glm::vec3 GetPoint(float t) const { return o + d * t; }
  };

  struct Plane
  {
    glm::vec3 o;
    glm::vec3 n;
  };

  struct CastHitResult
  {
    float t = 0.0f;
    EkBody *body = nullptr;
    glm::vec3 normal;
    glm::vec3 position;
    bool success = false;

    CastHitResult() :
      normal(0.0f), position(0.0f) {}
  };

  // ---- Box vs box contact generation (SAT + Sutherland-Hodgman clipping) ----

  struct ClipPlane
  {
    glm::vec3 point{0.0f};
    glm::vec3 normal{0.0f, 1.0f, 0.0f};
  };

  // Step 1: clip an ordered polygon against a single plane.
  std::vector<glm::vec3> ClipPolygonAgainstPlane(const std::vector<glm::vec3> &polygon, const ClipPlane &plane);

  // Step 2: separating axis test that also reports which axis won.
  enum class ESatAxis { FaceA, FaceB, EdgeEdge };

  struct SatResult
  {
    bool overlapping = false;
    ESatAxis type = ESatAxis::FaceA;
    int indexA = -1; // which local axis of A (0, 1, 2); -1 when not involved
    int indexB = -1; // which local axis of B (0, 1, 2); -1 when not involved
    float penetration = 0.0f;
    glm::vec3 normal{0.0f}; // unit length, points from A to B
  };

  SatResult BoxBoxSAT(const OBB &a, const OBB &b);

  // Step 3: reference and incident face selection.
  struct BoxFace
  {
    int axis = 0; // which local axis of the box the face is on (0, 1, 2)
    float sign = 1.0f; // +1 or -1: which of the two faces on that axis
    glm::vec3 normal{0.0f}; // outward, unit length, world space
    glm::vec3 center{0.0f}; // world space
    std::array<glm::vec3, 4> vertices{}; // world space, counter-clockwise seen from outside
  };

  // One face of a box, in world space.
  BoxFace GetBoxFace(const OBB &box, int axis, float sign);

  struct FacePair
  {
    BoxFace reference; // on the box that owns the winning axis, facing the other box
    BoxFace incident; // on the other box, the face most opposed to the reference face
    bool flipped = false; // true when the reference face is on B
  };

  // Only valid for a face axis (FaceA or FaceB). The edge-edge axis is handled in step 6.
  FacePair SelectFaces(const OBB &a, const OBB &b, const SatResult &sat);

  // Step 4: clip the incident face against the reference face's 4 side planes.

  // The incident face's 4 vertices, clipped one after another against the 4 side planes of the
  // reference face. Returns 0 to 8 points. Points above the reference face are still in (step 5).
  std::vector<glm::vec3> ClipIncidentFace(const FacePair &faces);

  // Step 5: keep the clipped points below the reference face, depth per point.
  // (ContactPoint and ContactManifold live in ContactManifold.h.)

  // Turns the clipped incident points (from ClipIncidentFace) into a manifold.
  // Only valid for a face axis (FaceA or FaceB). The edge-edge axis is handled in step 6.
  ContactManifold BuildFaceManifold(const FacePair &faces, const SatResult &sat, const std::vector<glm::vec3> &clipped);

  // Step 6: edge-edge case.

  // One edge of a box as a segment: midpoint, unit direction, half length.
  struct BoxEdge
  {
    glm::vec3 midpoint{0.0f}; // world space
    glm::vec3 direction{0.0f}; // unit length, the box's local axis `axis` in world space
    float halfLength = 0.0f; // the box's half extent on that axis
  };

  // Of the 4 edges of `box` parallel to local axis `axis`, the one furthest along `towards`.
  BoxEdge GetSupportingEdge(const OBB &box, int axis, const glm::vec3 &towards);

  // Closest points between segments e0 and e1. Writes pointOnE0 and pointOnE1.
  // The edges must not be parallel (SAT already skips parallel edge pairs).
  void ClosestPointsBetweenEdges(const BoxEdge &e0, const BoxEdge &e1, glm::vec3 &pointOnE0, glm::vec3 &pointOnE1);

  // Only valid for the edge-edge axis (sat.type == EdgeEdge). Returns 1 point.
  ContactManifold BuildEdgeManifold(const OBB &a, const OBB &b, const SatResult &sat);

  // Step 7: manifold reduction to 4 points.

  // Keeps at most 4 points that cover the contact area well:
  //  #1 the deepest point
  //  #2 the point farthest from #1
  //  #3 the point with the largest positive signed area dot(cross(#2 - #1, p - #1), normal)
  //  #4 the point with the most negative signed area (the other side of the #1-#2 line)
  // Manifolds with 4 or fewer points are returned unchanged. The 4 points come back in
  // polygon order: #1, #3, #2, #4.
  ContactManifold ReduceManifold(const ContactManifold &manifold);

  namespace CollisionSystem
  {
    bool RayVsOBB(Ray ray, const BoxColliderComponent &boxCollider, float &t);


    bool RayVsSphere(Ray ray, glm::vec3 sphereCenter, float radius,
                     glm::vec2 &points);


    // Contact functions. Each returns true when the shapes overlap and fills `manifold`.
    // The manifold normal points from the first shape to the second.

    // 1 point, at the midpoint between the two centres.
    bool SphereVsSphere(const Sphere &a, const Sphere &b, ContactManifold &manifold);

    // 1 point, the closest point on the box to the sphere centre.
    // Reports no contact when the sphere centre is inside the box.
    bool BoxVsSphere(const OBB &box, const Sphere &sphere, ContactManifold &manifold);

    // Up to 4 points, depth per point.
    bool BoxVsBox(const OBB &a, const OBB &b, ContactManifold &manifold);
  } // namespace CollisionSystem
} // namespace Eklavya::Physics

#endif /* CollisionSystem_hpp */
