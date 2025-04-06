#include "Collider.h"

using namespace Eklavya::Physics;

BaseColliderComponent::BaseColliderComponent(EkActor &owner, EColliderType type) :
	EkComponent(owner, CoreComponentIds::COLLIDER_COMPONENT_ID), mType(type), mGroupFlag(0) {}

void BaseColliderComponent::UpdateTransform(glm::vec3 pos, glm::quat theta)
{
	glm::mat3 rotation = glm::toMat3(theta);
	glm::mat4 translation = glm::translate(glm::mat4(1.0f), pos + mOffset);
	mColliderTR = translation * glm::mat4(rotation);
}

BaseColliderComponent::~BaseColliderComponent() {}

BoxColliderComponent::BoxColliderComponent(EkActor &owner) :
	BaseColliderComponent(owner, EColliderType::BOX) {}

OBB BoxColliderComponent::GetOBB() const
{
	OBB obb;
	obb.center = GetPosition();
	obb.halfExtents = mHalfSize;
	obb.rotation = GetRotationMatrix();
	obb.worldTransform = GetWorldMatrix();

	return obb;
}


BoxColliderComponent::~BoxColliderComponent() {}

void BoxColliderComponent::SetHalfSize(glm::vec3 halfSize)
{
	mHalfSize = halfSize / 2.0f;
}

SphereColliderComponent::SphereColliderComponent(EkActor &owner) :
	BaseColliderComponent(owner, EColliderType::SPHERE) {}

SphereColliderComponent::~SphereColliderComponent()
{
	printf("destroyed sphere!");
}

void SphereColliderComponent::SetRadius(float radius)
{
	mRadius = radius;
}

Sphere SphereColliderComponent::GetSphere() const
{
	Sphere sphere;
	sphere.center = GetPosition();
	sphere.radius = mRadius;
	return sphere;
}
