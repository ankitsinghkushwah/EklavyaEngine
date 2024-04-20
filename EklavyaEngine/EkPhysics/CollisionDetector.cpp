#include "CollisionDetector.h"
#include "Collider.h"
#include <sstream>
#include "../Helpers.h"
#include <limits>
#include "../Random.h"
#include "EkBody.h"

namespace Eklavya::Physics {
float GetProjectedLength(std::shared_ptr<BoxCollider> box, glm::vec3 axis) {
  auto body = box->GetBody();
  glm::vec3 scale =
      box->GetHalfSize(); // considering box has uniform haflsize for all axes
  float x = scale.x * glm::abs(glm::dot(body->GetAxis(0), axis));
  float y = scale.y * glm::abs(glm::dot(body->GetAxis(1), axis));
  float z = scale.z * glm::abs(glm::dot(body->GetAxis(2), axis));
  float length = (x + y + z);
  return length;
}

// INTERSECTION TESTS
bool OverlapOnAxis(std::shared_ptr<BoxCollider> one,
                   std::shared_ptr<BoxCollider> two, glm::vec3 axis) {
  if (glm::length2(axis) < .0001f)
    return true;
  axis = glm::normalize(axis);

  glm::vec3 box1Pos = one->GetBody()->GetAxis(3);
  glm::vec3 box2Pos = two->GetBody()->GetAxis(3);
  glm::vec3 toCenter = box2Pos - box1Pos;

  float projectedMidlineLen = abs(glm::dot(toCenter, axis));
  float projectedBoxOneLen = GetProjectedLength(one, axis);
  float projectedBoxTwoLen = GetProjectedLength(two, axis);
  return projectedMidlineLen < (projectedBoxOneLen + projectedBoxTwoLen);
}

bool BoxAndBox(std::shared_ptr<BoxCollider> one,
               std::shared_ptr<BoxCollider> two) {
  bool collision = OverlapOnAxis(one, two, one->GetBody()->GetAxis(0)) &&
                   OverlapOnAxis(one, two, one->GetBody()->GetAxis(1)) &&
                   OverlapOnAxis(one, two, one->GetBody()->GetAxis(2)) &&

                   OverlapOnAxis(one, two, two->GetBody()->GetAxis(0)) &&
                   OverlapOnAxis(one, two, two->GetBody()->GetAxis(1)) &&
                   OverlapOnAxis(one, two, two->GetBody()->GetAxis(2)) &&

                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(0),
                                            two->GetBody()->GetAxis(0))) &&
                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(0),
                                            two->GetBody()->GetAxis(1))) &&
                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(0),
                                            two->GetBody()->GetAxis(2))) &&

                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(1),
                                            two->GetBody()->GetAxis(0))) &&
                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(1),
                                            two->GetBody()->GetAxis(1))) &&
                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(1),
                                            two->GetBody()->GetAxis(2))) &&

                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(2),
                                            two->GetBody()->GetAxis(0))) &&
                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(2),
                                            two->GetBody()->GetAxis(1))) &&
                   OverlapOnAxis(one, two,
                                 glm::cross(one->GetBody()->GetAxis(2),
                                            two->GetBody()->GetAxis(2)));

  return collision;
}

bool BoxAndSphere(std::shared_ptr<BoxCollider> box,
                  std::shared_ptr<SphereCollider> sphere) {
  glm::vec3 localSpherePosition =
      box->GetBody()->GetPointInLocalSpace(sphere->GetBody()->GetPosition());

  bool xIsGreater =
      abs(localSpherePosition.x) - sphere->GetRadius() > box->GetHalfSize().x;
  bool yIsGreater =
      abs(localSpherePosition.y) - sphere->GetRadius() > box->GetHalfSize().y;
  bool zIsGreater =
      abs(localSpherePosition.z) - sphere->GetRadius() > box->GetHalfSize().z;

  if (!xIsGreater && !yIsGreater && !zIsGreater)
    return true;
  return false;
}

bool SphereAndSphere(std::shared_ptr<SphereCollider> one,
                     std::shared_ptr<SphereCollider> two) {
  glm::vec3 midline =
      one->GetBody()->GetPosition() - two->GetBody()->GetPosition();
  float totalRadius = one->GetRadius() + two->GetRadius();
  return glm::length2(midline) < totalRadius * totalRadius;
}

// Contact Generation
bool SphereAndSphere(std::shared_ptr<SphereCollider> sphereOne,
                     std::shared_ptr<SphereCollider> sphereTwo,
                     std::vector<ContactData> &pContacts) {
  if (!SphereAndSphere(sphereOne, sphereTwo))
    return false;

  ContactData newContact;
  newContact.bodyA = sphereOne->GetBody();
  newContact.bodyB = sphereTwo->GetBody();

  glm::vec3 p1 = newContact.bodyA->GetPosition();
  glm::vec3 p2 = newContact.bodyB->GetPosition();

  glm::vec3 toSphereTwo = p2 - p1;
  float size = glm::length(toSphereTwo);

  newContact.normal = -1.0f * (toSphereTwo / size);
  newContact.point = p1 + (toSphereTwo / 2.0f);
  newContact.depth = (sphereOne->GetRadius() + sphereTwo->GetRadius()) - size;

  pContacts.push_back(newContact);

  return true;
}

bool SphereAndBox(std::shared_ptr<BoxCollider> box,
                  std::shared_ptr<SphereCollider> sphere,
                  std::vector<ContactData> &pContacts) {
  assert(box);
  assert(sphere);

  // early test
  if (!BoxAndSphere(box, sphere))
    return false;

  ContactData newContact;
  newContact.bodyA = sphere->GetBody();
  newContact.bodyB = box->GetBody();

  glm::vec3 relativePos =
      box->GetBody()->GetPointInLocalSpace(sphere->GetBody()->GetPosition());
  glm::vec3 halfSize = box->GetHalfSize();
  glm::vec3 closestPoint = glm::clamp(relativePos, -1.0f * halfSize, halfSize);

  glm::vec3 closestPointWorld =
      box->GetBody()->GetPointInWorldSpace(closestPoint);

  float dist = glm::length2(closestPoint - relativePos);

  if (dist > sphere->GetRadius() * sphere->GetRadius() || dist < .000005f)
    return false;

  newContact.point = closestPointWorld;
  newContact.normal =
      glm::normalize(sphere->GetBody()->GetPosition() - closestPointWorld);
  newContact.depth = sphere->GetRadius() - sqrt(dist);

  pContacts.push_back(newContact);

  return true;
}

float GetPenOnAxis(std::shared_ptr<BoxCollider> box1,
                   std::shared_ptr<BoxCollider> box2, const glm::vec3 &toCenter,
                   const glm::vec3 axis) {
  float boxOneProjectionLen = GetProjectedLength(box1, axis);
  float boxTwoProjectionLen = GetProjectedLength(box2, axis);
  float toCenterProjectionLen = glm::abs(glm::dot(toCenter, axis));
  return (boxOneProjectionLen + boxTwoProjectionLen) - toCenterProjectionLen;
}

bool CheckAxisForSmallestPen(std::shared_ptr<BoxCollider> box1,
                             std::shared_ptr<BoxCollider> box2,
                             const glm::vec3 &toCenter, glm::vec3 axis,
                             unsigned index, unsigned &bestAxis,
                             float &smallestPen) {
  // checking if axis were parallel when taking cross products
  if (glm::length2(axis) < 0.0001f)
    return true;
  axis = glm::normalize(axis);

  float penetration = GetPenOnAxis(box1, box2, toCenter, axis);
  if (penetration < 0)
    return false;
  if (penetration < smallestPen) {
    smallestPen = penetration;
    bestAxis = index;
  }
  return true;
}

void PointFaceContactForBoxes(std::shared_ptr<BoxCollider> box1,
                              std::shared_ptr<BoxCollider> box2,
                              const glm::vec3 &toCenter, float pen,
                              int axisIndex, ContactData &newContact) {
  glm::vec3 normal = box1->GetBody()->GetAxis(axisIndex);
  if (glm::dot(normal, toCenter) > 0) {
    normal *= -1.0f;
  }

  // determining the box2 vertex which is in contact
  glm::vec3 localContact = box2->GetHalfSize();

  if (glm::dot(normal, box2->GetBody()->GetAxis(0)) < 0)
    localContact.x *= -1.0f;
  if (glm::dot(normal, box2->GetBody()->GetAxis(1)) < 0)
    localContact.y *= -1.0f;
  if (glm::dot(normal, box2->GetBody()->GetAxis(2)) < 0)
    localContact.z *= -1.0f;

  newContact.normal = normal;
  newContact.point = box2->GetBody()->GetPointInWorldSpace(localContact);
}

bool BoxAndBox(std::shared_ptr<BoxCollider> box1,
               std::shared_ptr<BoxCollider> box2,
               std::vector<ContactData> &pContacts) {
  unsigned bestAxis = 0xffffff;
  float smallestPen = MAX_FLOAT;
  glm::vec3 toCenter =
      box2->GetBody()->GetAxis(3) - box1->GetBody()->GetAxis(3);
  // get smallest penetration

  bool overlap =
      CheckAxisForSmallestPen(box1, box2, toCenter, box1->GetBody()->GetAxis(0),
                              0, bestAxis, smallestPen) &&

      CheckAxisForSmallestPen(box1, box2, toCenter, box1->GetBody()->GetAxis(1),
                              1, bestAxis, smallestPen) &&

      CheckAxisForSmallestPen(box1, box2, toCenter, box1->GetBody()->GetAxis(2),
                              2, bestAxis, smallestPen) &&

      CheckAxisForSmallestPen(box1, box2, toCenter, box2->GetBody()->GetAxis(0),
                              3, bestAxis, smallestPen) &&

      CheckAxisForSmallestPen(box1, box2, toCenter, box2->GetBody()->GetAxis(1),
                              4, bestAxis, smallestPen) &&

      CheckAxisForSmallestPen(box1, box2, toCenter, box2->GetBody()->GetAxis(2),
                              5, bestAxis, smallestPen);

  if (!overlap || bestAxis == 0xffffff)
    return false;

  // assert(bestAxis != 0xffffff);

  /* Box2 point is colliding with the face of box1*/
  if (bestAxis < 3) {
    ContactData newContact;
    newContact.bodyA = box1->GetBody();
    newContact.bodyB = box2->GetBody();
    newContact.depth = smallestPen;
    PointFaceContactForBoxes(box1, box2, toCenter, smallestPen, bestAxis,
                             newContact);
    pContacts.push_back(newContact);
    return true;
  }
  /* Box1 point is colliding with the face of box2*/
  else if (bestAxis < 6) {
    ContactData newContact;
    newContact.bodyA = box2->GetBody();
    newContact.bodyB = box1->GetBody();
    newContact.depth = smallestPen;
    PointFaceContactForBoxes(box2, box1, toCenter * -1.0f, smallestPen,
                             bestAxis - 3, newContact);
    pContacts.push_back(newContact);
    return true;
  }
  return false;
}
} // namespace Eklavya::Physics
