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
	void AddOrUpdateConstraint(std::vector<ContactConstraint> &constraints, EkBody &bodyA, EkBody &bodyB,
	                           glm::vec3 point, glm::vec3 collisionNormal, float penetration)
	{
		auto iter = std::find_if(constraints.begin(), constraints.end(),
		                         [&bodyA, &bodyB](const ContactConstraint &constraint)
		                         {
			                         return constraint.BodiesMatch(bodyA, bodyB);
		                         });

		if (iter == constraints.end())
		{
			ContactConstraint &constraint = constraints.emplace_back(bodyA, bodyB);
			constraint.AddCollisionPoint(point, collisionNormal, penetration);
		}
		else
		{
			iter->AddCollisionPoint(point, collisionNormal, penetration);
		}
	}


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

				CollisionPointInfo info;
				bool IsColliding = CollisionSystem::SphereAndBox(box->GetOBB(), sphere, info);
				if (IsColliding && glm::length2(info.location - sphereCenter) < glm::length2(
					    nearestContact - sphereCenter))
				{
					found = true;
					nearestContact = info.location;
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

				std::vector<glm::vec3> collisionPointsForPair;
				float maxPen = 0.0f;
				glm::vec3 collisionNormal;

				if (firstCollider->GetType() == EColliderType::BOX && secondCollider->GetType() == EColliderType::BOX)
				{
					const BoxColliderComponent *box1 = static_cast<const BoxColliderComponent *>(firstCollider);
					const BoxColliderComponent *box2 = static_cast<const BoxColliderComponent *>(secondCollider);

					bool IsColliding = CollisionSystem::BoxAndBox(box1->GetOBB(), box2->GetOBB(),
					                                              collisionPointsForPair, maxPen, collisionNormal);

					if (IsColliding)
					{
						for (glm::vec3 collisionPoint: collisionPointsForPair)
						{
							AddOrUpdateConstraint(constraints, *firstBody, *secondBody, collisionPoint, collisionNormal,
							                      maxPen);
						}
					}
				}
				else if (firstCollider->GetType() == EColliderType::SPHERE && secondCollider->GetType() ==
				         EColliderType::SPHERE)
				{
					const SphereColliderComponent *sphere1 = static_cast<const SphereColliderComponent *>(
						firstCollider);
					const SphereColliderComponent *sphere2 = static_cast<const SphereColliderComponent *>(
						secondCollider);

					CollisionPointInfo info;

					bool IsColliding = CollisionSystem::SphereAndSphere(sphere1->GetSphere(), sphere2->GetSphere(),
					                                                    info);

					if (IsColliding)
						AddOrUpdateConstraint(constraints, *firstBody, *secondBody, info.location, info.normal,
						                      info.peneteration);
				}

				else if (firstCollider->GetType() == EColliderType::BOX && secondCollider->GetType() ==
				         EColliderType::SPHERE)
				{
					const SphereColliderComponent *sphere = static_cast<const SphereColliderComponent *>(
						secondCollider);
					const BoxColliderComponent *box = static_cast<const BoxColliderComponent *>(firstCollider);

					CollisionPointInfo info;

					bool IsColliding = CollisionSystem::SphereAndBox(box->GetOBB(), sphere->GetSphere(), info);

					if (IsColliding)
						AddOrUpdateConstraint(constraints, *secondBody, *firstBody, info.location, info.normal,
						                      info.peneteration);
				}

				else if (firstCollider->GetType() == EColliderType::SPHERE && secondCollider->GetType() ==
				         EColliderType::BOX)
				{
					const SphereColliderComponent *sphere = static_cast<const SphereColliderComponent *>(firstCollider);
					const BoxColliderComponent *box = static_cast<const BoxColliderComponent *>(secondCollider);

					CollisionPointInfo info;

					bool IsColliding = CollisionSystem::SphereAndBox(box->GetOBB(), sphere->GetSphere(), info);

					if (IsColliding)
						AddOrUpdateConstraint(constraints, *firstBody, *secondBody, info.location, info.normal,
						                      info.peneteration);
				}
			};
		}

		return constraints;
	}
} // namespace Eklavya::Physics::ContactGenerator