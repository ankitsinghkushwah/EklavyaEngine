#include "PhysicsWorld.h"
#include "../Event/Events.h"
#include "../Event/EventType.h"
#include "../Event/EventDispatcher.h"
#include "../Components/GameActor.h"
#include "../Helpers.h"
#include "Collider.h"
#include "CollisionDetector.h"
#include "EkBody.h"
#include <algorithm>
#include "Scene/EkActor.h"
#include "Components/TransformComponent.h"
#include <glm/gtc/matrix_access.hpp>

namespace Eklavya::Physics
{

  uint32_t gDebugFeaturesEnabled = EFeatureEnabled::ALL;

  World::World() {}

  World::~World() {}

  void World::AddBody(EkBody *body) { mBodies.push_back(body); }

  void World::RemoveBody(EkBody *collider) {}

  void World::GenerateContacts(std::shared_ptr<ICollider> firstCollider,
                               std::shared_ptr<ICollider> secondCollider,
                               std::vector<ContactData>  &contacts) const
  {
    if (firstCollider->GetType() == EColliderType::BOX &&
        secondCollider->GetType() == EColliderType::BOX)
      {
        std::shared_ptr<BoxCollider> box1 =
            std::static_pointer_cast<BoxCollider>(firstCollider);
        std::shared_ptr<BoxCollider> box2 =
            std::static_pointer_cast<BoxCollider>(secondCollider);

        BoxAndBox(box1, box2, contacts);
      }

    if (firstCollider->GetType() == EColliderType::BOX &&
        secondCollider->GetType() == EColliderType::SPHERE)
      {
        std::shared_ptr<BoxCollider> box =
            std::static_pointer_cast<BoxCollider>(firstCollider);
        std::shared_ptr<SphereCollider> sphere =
            std::static_pointer_cast<SphereCollider>(secondCollider);

        SphereAndBox(box, sphere, contacts);
      }

    if (firstCollider->GetType() == EColliderType::SPHERE &&
        secondCollider->GetType() == EColliderType::BOX)
      {
        std::shared_ptr<BoxCollider> box =
            std::static_pointer_cast<BoxCollider>(secondCollider);
        std::shared_ptr<SphereCollider> sphere =
            std::static_pointer_cast<SphereCollider>(firstCollider);

        SphereAndBox(box, sphere, contacts);
      }

    if (firstCollider->GetType() == EColliderType::SPHERE &&
        secondCollider->GetType() == EColliderType::SPHERE)
      {
        std::shared_ptr<SphereCollider> sphere1 =
            std::static_pointer_cast<SphereCollider>(secondCollider);
        std::shared_ptr<SphereCollider> sphere0 =
            std::static_pointer_cast<SphereCollider>(firstCollider);

        SphereAndSphere(sphere0, sphere1, contacts);
      }
  }

  void World::Step(float delta)
  {
    for (auto *body : mBodies)
      body->Integrate(delta);

    // Generate Contacts
    std::vector<ContactData> contacts;

    int                      iter = 0;

    for (int idx = 0; idx < mBodies.size(); ++idx)
      {
        std::shared_ptr<ICollider> firstCollider = mBodies[idx]->GetCollider();

        if (firstCollider->GetBody()->IsAwake() == false)
          {
            continue;
          }

        for (auto second = idx + 1; second < mBodies.size(); ++second)
          {
            iter++;
            std::shared_ptr<ICollider> secondCollider =
                mBodies[second]->GetCollider();
            if (secondCollider->GetBody()->IsAwake() == false)
              {
                continue;
              }
            if (firstCollider == secondCollider)
              continue;

            GenerateContacts(firstCollider, secondCollider, contacts);
          };
      }

    for (ContactData &manifold : contacts)
      {
        mConstraints.emplace_back(manifold);
      }

    for (const auto &body : mBodies)
      {
        body->ApplyGravityForce();
      }

    for (int i = 0; i < 50; i++)
      {
        for (auto &constraint : mConstraints)
          {
            constraint.Solve();
          }
      }

    for (auto &constraint : mConstraints)
      constraint.PostSolve();

    mConstraints.clear();
  }

  bool World::RayVSOBB(Ray ray, std::shared_ptr<BoxCollider> boxCollider,
                       float &t) const
  {

    ray.d = glm::normalize(ray.d);

    float     tMin = -FLT_MAX;
    float     tMax = FLT_MAX;

    glm::mat4 worldMatrix =
        boxCollider->GetBody()->mOwner->Transform()->GetWorldMatrix();

    glm::vec3 center = worldMatrix[3];

   

    for (int i = 0; i < 3; i++)
      {
        float     t1 = -FLT_MAX;
        float     t2 = FLT_MAX;

        glm::vec3 normal = glm::column(worldMatrix, i);
        float     l =
            glm::length(normal) *
            0.5f; // half of the length of the world transformation axes because
                  // our bounding box vertices are bound to -0.5 -> 0.5
        normal = glm::normalize(normal);

        Plane nearPlane;
        nearPlane.o = center + (normal * l);
        nearPlane.n = normal;

        Plane farPlane;
        farPlane.o = center - (normal * l);
        farPlane.n = normal;

        RayVSPlane(ray, nearPlane, t1);
        RayVSPlane(ray, farPlane, t2);


        //        if (mDebugRenderer)
        //          {
        //            float     rad = .2f;
        //            glm::vec4 col(normal, 1.0f);
        //            float     nLen = 10.0f;
        //            mDebugRenderer->AddSphere(nearPlane.o, rad, col);
        //            mDebugRenderer->AddLine(nearPlane.o, nearPlane.o - normal
        //            * l,
        //                                    col, .1f);
        //            mDebugRenderer->AddSphere(farPlane.o, rad, col);
        //            mDebugRenderer->AddLine(farPlane.o, farPlane.o + normal *
        //            l, col,
        //                                    .1f);
        //       }

        if (t1 > t2)
          {
            std::swap(t1, t2);
          }

        if (t1 > tMin)
          {
            tMin = t1;
          }

        if (t2 < tMax)
          {
            tMax = t2;
          }

        if (tMax < tMin)
          return false;
      }

    t = tMin;
    return true;
  }

  bool World::RayVSSphere(Ray ray, glm::vec3 sphereCenter, float radius,
                          glm::vec2 &points) const
  {
    glm::vec3 L = ray.o - sphereCenter;

    float     a = glm::dot(ray.d, ray.d);
    float     b = 2.0f * glm::dot(ray.d, L);
    float     c = glm::dot(L, L) - radius * radius;

    float     discr = b * b - 4 * a * c;
    if (discr < 0)
      return false;

    else if (discr == 0)
      points[0] = points[1] = -0.5 * b / a;
    else
      {
        points[0] = -0.5 * (b + sqrt(discr));
        points[1] = -0.5 * (b - sqrt(discr));
      }

    if (points[0] > points[1])
      std::swap(points[0], points[1]);

    return true;
  }

  CastHitResult World::SphereCast(glm::vec3 o, glm::vec3 direction,
                                  float radius, float range,
                                  int ignoreGroupFlag) const
  {
    CastHitResult result = RayCast(o, direction, range, ignoreGroupFlag);
    if (result.success == false)
      return result;

    auto      collider = std::make_shared<Physics::SphereCollider>();
    float     thres = 1.0f;
    glm::vec3 center = result.position - ((direction * radius) * thres);

    collider->SetRadius(radius);
    collider->SetGroupIndex(0);
    auto body = std::make_shared<EkBody>(collider);
    body->SetMass(FLT_MAX);
    body->SetPos(center);
    collider->SetBody(body.get());

    glm::vec3 closestContactPoint = glm::vec3(FLT_MAX);

    for (auto &body : mBodies)
      {
        std::vector<ContactData> newContacts;
        GenerateContacts(collider, body->GetCollider(), newContacts);
        if (newContacts.empty() == false)
          //  mContactsSphereCast.insert(mContactsSphereCast.end(),
          //  newContacts.begin(), newContacts.end());
          for (auto &contact : newContacts)
            {
              if (glm::abs((contact.point - center).y) <
                  glm::abs((closestContactPoint - center).y))
                {
                  closestContactPoint = contact.point;
                }
            }
      }

    //    if (mContactsSphereCast.empty() == false)
    //      {
    //        result.position = closestContactPoint;
    //      }

    return result;
  }

  CastHitResult World::RayCast(glm::vec3 o, glm::vec3 d, float maxRange,
                               int ignoreGroupFlag) const
  {
    Ray           ray{o, d, maxRange};
    CastHitResult result;

    float         t = 0.0f;
    int           index = -1;
    float         shortestT = maxRange;
    std::shared_ptr<ICollider> hitCollider = nullptr;

    for (int i = 0; i < mBodies.size(); i++)
      {
        auto collider = mBodies[i]->GetCollider();

        if (collider->GroupFlag() == ignoreGroupFlag)
          continue;

        bool success = false;

        if (collider->GetType() == EColliderType::BOX)
          {
            std::shared_ptr<BoxCollider> boxCollider =
                std::static_pointer_cast<BoxCollider>(
                    mBodies[i]->GetCollider());
            success = RayVSOBB(ray, boxCollider, t);
          }
        else if (collider->GetType() == EColliderType::SPHERE)
          {
            std::shared_ptr<SphereCollider> sphereCollider =
                std::static_pointer_cast<SphereCollider>(
                    mBodies[i]->GetCollider());
            glm::vec2 points;
            success = RayVSSphere(ray, sphereCollider->GetBody()->GetPosition(),
                                  sphereCollider->GetRadius(), points);
            if (success)
              {
                t = points[0];
              }
          }

        if (success && t < shortestT)
          {
            shortestT = t;
            index = i;
            hitCollider = collider;
          }
      }

    if (index != -1)
      {
        result.t = shortestT;
        result.success = true;
        result.position = o + d * shortestT;
        result.body = hitCollider->GetBody();
      }
    else
      {
        result.position = o + d * maxRange;
      }

    return result;
  }

  void World::OnDebugDraw(Renderer::DebugRenderer &debugRenderer)
  {
    if (!mDebugRenderer)
      mDebugRenderer = &debugRenderer;
  }

} // namespace Eklavya::Physics
