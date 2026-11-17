//
//  ContactGenerator.cpp
//  EklavyaEngine
//
//  Created by Ankit Singh Kushwah on 02/01/2025.
//

#include "ContactGenerator.h"
#include "Collider.h"
#include "Scene/EkActor.h"
#include "CollisionSystem.h"

namespace Eklavya::Physics::ContactGenerator
{
	bool GetNearestContactToSphere(const std::vector<EkBody *> &bodies, glm::vec3 sphereCenter, float sphereRadius,
	                               glm::vec3 &nearestContact)
	{
		bool found = false;
		float earlyOutCheckRadius = sphereRadius + 5.0f;
		Sphere sphere;
		sphere.center = sphereCenter;
		sphere.radius = sphereRadius;


		for (EkBody *body: bodies)
		{
			const BaseColliderComponent *collider = body->GetOwner().GetComponent<BaseColliderComponent>(
				CoreComponentIds::COLLIDER_COMPONENT_ID);

			if (collider->GetType() == EColliderType::BOX)
			{
				const BoxColliderComponent *box = static_cast<const BoxColliderComponent *>(collider);

				if (glm::length2(box->GetPosition() - sphereCenter) > glm::pow(earlyOutCheckRadius, 2))
				{
					continue;
				}

				ContactManifold manifold;
				if (CollisionSystem::BoxVsSphere(box->GetOBB(), sphere, manifold) == false)
				{
					continue;
				}

				const glm::vec3 contactPosition = manifold.points[0].position;
				if (glm::length2(contactPosition - sphereCenter) < glm::length2(nearestContact - sphereCenter))
				{
					found = true;
					nearestContact = contactPosition;
				}
			}
		}

		return found;
	}

	std::vector<ContactConstraint> CreateConstraints(const std::vector<EkBody *> &bodies)
	{
		std::vector<ContactConstraint> constraints;

		for (int first = 0; first < bodies.size(); ++first)
		{
			EkBody *firstBody = bodies[first];

			for (int second = first + 1; second < bodies.size(); ++second)
			{
				EkBody *secondBody = bodies[second];

				const BaseColliderComponent *firstCollider = firstBody->GetOwner().GetComponent<BaseColliderComponent>(
					CoreComponentIds::COLLIDER_COMPONENT_ID);
				const BaseColliderComponent *secondCollider = secondBody->GetOwner().GetComponent<
					BaseColliderComponent>(CoreComponentIds::COLLIDER_COMPONENT_ID);

				const EColliderType firstType = firstCollider->GetType();
				const EColliderType secondType = secondCollider->GetType();

				// Each CollisionSystem *Vs* function gives a normal from its first shape to its
				// second, so the body passed as the first shape becomes body A of the constraint.
				ContactManifold manifold;
				EkBody *bodyA = firstBody;
				EkBody *bodyB = secondBody;
				bool isColliding = false;

				if (firstType == EColliderType::BOX && secondType == EColliderType::BOX)
				{
					isColliding = CollisionSystem::BoxVsBox(
						static_cast<const BoxColliderComponent *>(firstCollider)->GetOBB(),
						static_cast<const BoxColliderComponent *>(secondCollider)->GetOBB(), manifold);
				}
				else if (firstType == EColliderType::SPHERE && secondType == EColliderType::SPHERE)
				{
					isColliding = CollisionSystem::SphereVsSphere(
						static_cast<const SphereColliderComponent *>(firstCollider)->GetSphere(),
						static_cast<const SphereColliderComponent *>(secondCollider)->GetSphere(), manifold);
				}
				else if (firstType == EColliderType::BOX && secondType == EColliderType::SPHERE)
				{
					isColliding = CollisionSystem::BoxVsSphere(
						static_cast<const BoxColliderComponent *>(firstCollider)->GetOBB(),
						static_cast<const SphereColliderComponent *>(secondCollider)->GetSphere(), manifold);
				}
				else if (firstType == EColliderType::SPHERE && secondType == EColliderType::BOX)
				{
					isColliding = CollisionSystem::BoxVsSphere(
						static_cast<const BoxColliderComponent *>(secondCollider)->GetOBB(),
						static_cast<const SphereColliderComponent *>(firstCollider)->GetSphere(), manifold);
					bodyA = secondBody; // the box
					bodyB = firstBody; // the sphere
				}

				if (isColliding && manifold.points.empty() == false)
				{
					constraints.emplace_back(*bodyA, *bodyB, manifold);
				}
			}
		}

		return constraints;
	}
} // namespace Eklavya::Physics::ContactGenerator