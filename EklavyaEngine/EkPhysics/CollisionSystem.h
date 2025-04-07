//
//  CollisionSystem.hpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#ifndef CollisionSystem_hpp
#define CollisionSystem_hpp

#include <glm/glm.hpp>
#include <EkPhysics/Constraints/ContactConstraint.h>

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

  namespace CollisionSystem
  {
    bool RayVsOBB(Ray ray, const BoxColliderComponent &boxCollider, float &t);


    bool RayVsSphere(Ray ray, glm::vec3 sphereCenter, float radius,
                     glm::vec2 &points);


    bool SphereAndSphere(const Sphere &sphereOne,
                         const Sphere &sphereTwo,
                         CollisionPointInfo &newContact);

    bool SphereAndBox(const OBB &box,
                      const Sphere &sphere,
                      CollisionPointInfo &newContact);

    bool BoxAndBox(const OBB &box1,
                   const OBB &box2,
                   std::vector<glm::vec3> &collisionPoints, float &penetration,
                   glm::vec3 &collisionNormal);
  } // namespace CollisionSystem
} // namespace Eklavya::Physics

#endif /* CollisionSystem_hpp */
