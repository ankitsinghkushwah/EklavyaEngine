#ifndef _INC_CONTACT_DATA_H_
#define _INC_CONTACT_DATA_H_

#include <glm/glm.hpp>
#include <vector>
#include <memory>
#include "Constraint.h"

class BoxCollider;
class SphereCollider;
class EkBody;

namespace Eklavya::Physics {

bool BoxAndBox(std::shared_ptr<BoxCollider> boxFirst,
               std::shared_ptr<BoxCollider> boxSecond);
bool BoxAndSphere(std::shared_ptr<BoxCollider> box,
                  std::shared_ptr<SphereCollider> sphere);
bool SphereAndSphere(std::shared_ptr<SphereCollider> sphereFirst,
                     std::shared_ptr<SphereCollider> sphereSecond);
float GetProjectedLength(std::shared_ptr<BoxCollider> box, glm::vec3 axis);

bool OverlapOnAxis(std::shared_ptr<BoxCollider> one,
                   std::shared_ptr<BoxCollider> two, glm::vec3 axis);

float GetPenOnAxis(std::shared_ptr<BoxCollider> box1,
                   std::shared_ptr<BoxCollider> box2, const glm::vec3 &toCenter,
                   const glm::vec3 axis);

bool CheckAxisForSmallestPen(std::shared_ptr<BoxCollider> box1,
                             std::shared_ptr<BoxCollider> box2,
                             const glm::vec3 &toCenter, glm::vec3 axis,
                             unsigned index, unsigned &bestAxis,
                             float &smallestPen);

void PointFaceContactForBoxes(std::shared_ptr<BoxCollider> box1,
                              std::shared_ptr<BoxCollider> box2,
                              const glm::vec3 &toCenter, float pen,
                              int axisIndex, ContactData &newContact);

bool SphereAndSphere(std::shared_ptr<SphereCollider> sphereOne,
                     std::shared_ptr<SphereCollider> sphereTwo,
                     std::vector<ContactData> &pContacts);
bool SphereAndBox(std::shared_ptr<BoxCollider> box,
                  std::shared_ptr<SphereCollider> sphere,
                  std::vector<ContactData> &pContacts);
bool BoxAndBox(std::shared_ptr<BoxCollider> box1,
               std::shared_ptr<BoxCollider> box2,
               std::vector<ContactData> &pContacts);
} // namespace Eklavya::Physics

#endif
