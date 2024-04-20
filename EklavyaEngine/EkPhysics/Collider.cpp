#include "Collider.h"
#include "../Helpers.h"

using namespace Eklavya::Physics;

ICollider::ICollider(EColliderType type) : m_Type(type), mGroupFlag(0) {}

ICollider::~ICollider() {}

BoxCollider::BoxCollider() : ICollider(EColliderType::BOX) {}

BoxCollider::~BoxCollider() {}

void BoxCollider::SetHalfSize(glm::vec3 halfSize) {
  m_HalfSize =
      glm::vec3(halfSize.x / 2.0f, halfSize.y / 2.0f, halfSize.z / 2.0f);
}

SphereCollider::SphereCollider() : ICollider(EColliderType::SPHERE) {}

SphereCollider::~SphereCollider() {}

void SphereCollider::SetRadius(float radius) { mRadius = radius; }
