#ifndef INC_PHYSICS_WORLD_H
#define INC_PHYSICS_WORLD_H

#include <memory>
#include <vector>
#include <map>
#include <string>
#include <glm/glm.hpp>
#include "Constraint.h"
#include <bitset>

#ifdef EKDEBUG
#include "Renderer/DebugRenderer.hpp"
#endif

// Forward Declarations

class IEventData;

namespace Eklavya::Physics
{
  class ICollider;
  class BoxCollider;
  class SphereCollider;

  class EkBody;

  enum EPhysicsGroupFlag
  {
    STATIC,
    OTHER_COLLIDABLES
  };

  struct Ray
  {
    glm::vec3 o;
    glm::vec3 d;
    float     range;
  };

  struct Plane
  {
    glm::vec3 o;
    glm::vec3 n;
  };

  struct CastHitResult

  {
    float     t;
    EkBody   *body = nullptr;
    glm::vec3 normal;
    glm::vec3 position;
    bool      success;
  };

  enum EFeatureEnabled
  {
    COLLISION_DETECTION = 1 << 1,
    CONTACT_RESOLUTION = 1 << 2,
    RESOLVE_PEN = 1 << 3,
    APPLY_GRAVITY = 1 << 4,
    INTEGRATION = 1 << 5,
    ALL = COLLISION_DETECTION | CONTACT_RESOLUTION | RESOLVE_PEN |
          APPLY_GRAVITY | INTEGRATION,
    NONE = 0
  };

  extern uint32_t gDebugFeaturesEnabled;

  class World
  {
  public:
    World();
    ~World();
    void          AddBody(EkBody *body);
    void          RemoveBody(EkBody *body);
    void          Step(float delta);

    CastHitResult RayCast(glm::vec3 o, glm::vec3 direction, float range,
                          int ignoreGroupFlag) const;

    CastHitResult SphereCast(glm::vec3 o, glm::vec3 direction, float radius,
                             float range, int ignoreGroupFlag) const;

    bool          RayVSOBB(Ray ray, std::shared_ptr<BoxCollider> boxCollider,
                           float &t) const;

    bool          RayVSSphere(Ray ray, glm::vec3 sphereCenter, float radius,
                              glm::vec2 &points) const;

#ifdef EKDEBUG
    Renderer::DebugRenderer *mDebugRenderer = nullptr;
    void OnDebugDraw(Renderer::DebugRenderer &debugRenderer);
#endif
  private:
    bool RayVSPlane(Ray ray, Plane plane, float &t) const
    {
      float denom = glm::dot(plane.n, ray.d);
      // ray is parallel to plan?
      if (glm::abs(denom) > 0.0f)
        {
          float num = glm::dot(plane.n, (plane.o - ray.o));
          t = num / denom;
          return true;
        }
      return false;
    }

    void GenerateContacts(std::shared_ptr<ICollider> firstCollider,
                          std::shared_ptr<ICollider> secondCollider,
                          std::vector<ContactData>  &contacts) const;

    std::vector<ContactConstraint> mConstraints;
    std::vector<EkBody *>          mBodies;
  };
} // namespace Eklavya::Physics

#endif
